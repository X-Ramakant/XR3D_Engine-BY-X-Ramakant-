# Core / FileSystem — Development Log

## Module Information

| Field | Value |
|---|---|
| **Module** | FileSystem |
| **Location** | `Engine/Core/FileSystem/` |
| **Main Module Group** | Core |
| **Version Phase** | V1 |
| **Internal Core Ordinal** | 5th of 15 Core systems implemented |
| **Status** | COMPLETE |

## Purpose

Provides the single, unified entry point for all file I/O, path
manipulation, virtual mount-point resolution, and file-change watching
in the XR3D Engine. Every function in this module reports failure via
a result type (`FileResult<T>` / `FileError`) rather than throwing an
exception, so routine failures (a missing config file, an unwritten
save directory on first run) never crash the Engine.

## Why Required

FileSystem is the fifth Core system built. It directly pays off two
TODOs left by earlier modules: `Core/Logging`'s `FileSink` and
`Core/Config`'s `ConfigLoader` both currently open `std::ifstream`/
`std::ofstream` directly, with a comment noting they should migrate to
`Core/FileSystem` once it exists. Beyond that, virtually every future
system (Asset Pipeline, Serialization, Save/Load, Scripting) will need
file access, and building a safe, exception-free, virtual-path-aware
foundation now avoids ad-hoc file-handling scattered across later
modules.

## Architecture Overview

```
                    FileSystemManager  (singleton, single entry point)
                          |
              ------------------------------------
              |               |                   |
     FileSystemRegistry  FileSystemWatcher   FileSystemPath
    (alias:// mounts)    (polling-based       (pure string
                          file-watch)          manipulation)
                          |
                  std::filesystem (C++17 standard library)
```

### Key design decisions

- **No exceptions anywhere in this module.** Every read-style operation
  returns `FileResult<T>` (a `success`/`error`/`value` triple); every
  write-style operation returns `FileError` directly. This matches the
  project-wide philosophy already established in Memory (null-safety)
  and Config (`GetOrDefault` never crashes on missing key/type
  mismatch) - a file-system, more than almost any other subsystem,
  routinely encounters "expected" failures (missing files, first-run
  directories) that must never be treated as exceptional.
- **Built on `std::filesystem` (C++17), not Platform.** Since
  `std::filesystem` is already cross-platform standard library, there
  was no need to wait for `Platform/Windows/FileSystem` to exist first.
  `Platform`'s eventual role here would be OS-native file-watch events
  (see Future Work) - a genuinely OS-specific capability that
  `std::filesystem` cannot provide, unlike basic read/write/list/delete
  which it already handles portably.
- **Virtual mount points (`"assets://..."`).** `FileSystemRegistry`
  resolves `"alias://rest/of/path"` into a real path via a registered
  mount, and passes anything else (no `"://"`, or an unregistered
  alias) through unchanged. This means `FileSystemManager`'s public API
  can be called unconditionally with either a real path or a virtual
  one - no separate "is this virtual?" branch needed at call sites, and
  callers can move `Assets/` anywhere on disk without touching code
  that references `"assets://..."`.
- **Polling-based watcher with the same reentrancy-safety pattern as
  `Core/Time`.** `FileSystemWatcher::PollAll()` collects due callbacks
  while holding its mutex, then fires them only after releasing it -
  identical reasoning to `TimeRegistry::UpdateAll()`: a watch callback
  that itself calls `Watch()`/`Unwatch()` (a realistic hot-reload
  pattern - e.g. reloading a config and re-registering a watch on a
  newly-referenced file) cannot deadlock on the same non-recursive
  mutex.
- **Generational `WatchHandle`**, the same slot-map-with-generation
  pattern as `Core/Time`'s `TimerHandle`, for the same reason: a
  destroyed watch's slot is recycled, and a generation counter reliably
  detects a stale handle rather than letting it silently refer to an
  unrelated watch.

## Dependencies

C++17 `<filesystem>` (standard library) plus `<fstream>`, `<mutex>`,
`<functional>`. No dependency on Memory, Logging, Config, Time, or
Platform.

## Files Added

**Engine (9 files):**
- `Engine/Core/FileSystem/FileSystemTypes.h` *(header-only)*
- `Engine/Core/FileSystem/FileSystemPath.h` / `.cpp`
- `Engine/Core/FileSystem/FileSystemRegistry.h` / `.cpp`
- `Engine/Core/FileSystem/FileSystemWatcher.h` / `.cpp`
- `Engine/Core/FileSystem/FileSystemManager.h` / `.cpp`

**Tests (1 file):**
- `Tests/Unit/Core/FileSystemTests.cpp`

**Integration (1 file):**
- `Integration/Scenarios/Core/FileSystemIntegrationTests.cpp`

## Files Modified

- `Main.cpp` - added `FileSystemManager::Get().Initialize()`/`Shutdown()`,
  and a demonstration that checks for an `Assets` folder and mounts it
  as `assets://` if present (logging a warning, not an error, if
  absent). Init order is Memory, Logging, Config, Time, then
  FileSystem; shutdown order is the exact reverse.

## Public API Summary

| Call | Purpose |
|---|---|
| `FileSystemManager::Get().Initialize()` / `Shutdown()` | Lifecycle |
| `ReadAllText(path)` / `WriteAllText(path, content)` | Text I/O |
| `ReadAllBytes(path)` / `WriteAllBytes(path, data)` | Binary I/O |
| `Exists(path)` / `IsDirectory(path)` | Queries |
| `CreateDirectory(path)` / `DeleteFile(path)` / `ListDirectory(path)` | Directory operations |
| `GetInfo(path)` | Size/type metadata |
| `RegisterMount(alias, realPath)` / `HasMount(alias)` | Virtual mount points |
| `Watch(path, callback)` / `Unwatch(handle)` / `PollWatches()` | File-change watching |

## Unit Testing

**File:** `Tests/Unit/Core/FileSystemTests.cpp`
**Result:** 13 / 13 passing

1. `FileSystem_Manager_InitializeAndShutdown`
2. `FileSystem_Path_ManipulationFunctions`
3. `FileSystem_WriteAndReadText_RoundTrip`
4. `FileSystem_ReadMissingFile_ReturnsGracefulNotFound`
5. `FileSystem_WriteAndReadBytes_RoundTrip`
6. `FileSystem_Exists_And_IsDirectory`
7. `FileSystem_CreateDirectory_Nested`
8. `FileSystem_DeleteFile_MissingFile_ReturnsGracefulNotFound`
9. `FileSystem_ListDirectory_ContainsCreatedFile`
10. `FileSystem_VirtualMount_ResolvesCorrectly`
11. `FileSystem_UnknownMountAlias_PassesThroughGracefully`
12. `FileSystem_Watcher_DetectsChangeAndFires`
13. `FileSystem_Watcher_ReentrantCallback_DoesNotDeadlock`

## Integration Testing

**File:** `Integration/Scenarios/Core/FileSystemIntegrationTests.cpp`
**Result:** 3 / 3 passing

1. `Integration_FileSystem_BootTimeAssetMountAndLoad` - registers an
   `"assets://"` mount and reads a config file through it, the way
   `Main.cpp` will once Asset loading is wired in
2. `Integration_FileSystem_ConfigHotReloadViaWatcher` - a cached string
   is refreshed only when the watcher reports the underlying file
   changed - the exact pattern `Core/Config` will use once
   FileSystem-based hot-reload is wired into it
3. `Integration_FileSystem_FirstRunCreatesSaveDirectoryStructure` -
   simulates two sessions: session 1 is a brand-new install creating a
   nested save directory and writing save data; session 2 confirms the
   data survived

Confirmed by user: all three CMake targets built and ran clean together.

## Known Issues

None currently open.

## Resolved Issues

None - no defects found during this module's Build Validation or
Testing phases. The design lessons from Memory (null-safety) and Time
(reentrancy-safe deferred callbacks) were applied proactively here
rather than discovered via a bug.

## Future Work (TODO markers left in code)

- Replace the polling-based `FileSystemWatcher` with OS-native events
  (e.g. Windows `ReadDirectoryChangesW`) via `Platform/Windows` once
  that exists - polling has a detection delay of up to one
  `PollAll()` interval and does not scale to very large watch counts.
- Migrate `Core/Logging`'s `FileSink` and `Core/Config`'s
  `ConfigLoader` to route their file access through
  `FileSystemManager` instead of raw `std::ifstream`/`std::ofstream`.
- Consider async I/O once Threading/JobSystem exist, for large asset
  loads that should not block the main thread.

## Git

| Field | Value |
|---|---|
| Commit | *(fill in after `git commit`)* |
| Tag | v0.6.0-filesystem |

## Status

**Module = COMPLETE.** All pipeline steps finished except Git Commit/Tag/Push.