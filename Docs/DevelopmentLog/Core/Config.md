# Core / Config — Development Log

## Module Information

| Field | Value |
|---|---|
| **Module** | Config |
| **Location** | `Engine/Core/Config/` |
| **Main Module Group** | Core |
| **Version Phase** | V1 |
| **Internal Core Ordinal** | 3rd of 15 Core systems implemented |
| **Status** | COMPLETE |

## Purpose

Provides the single, unified entry point for reading and writing engine
and game settings. Every subsystem reads its configuration through
`ConfigManager` rather than hardcoding values, so settings can live in a
plain text file, be changed without recompiling, and persist across
Engine restarts.

## Why Required

Config is the third Core system built, right after Memory and Logging.
Time, FileSystem, and later feature modules (Rendering, Physics, Audio)
will all need tunable settings (window size, target frame rate, gravity,
volume, etc.). Building Config now means those systems can read their
settings from day one instead of hardcoding values that would need to
be retrofitted into a config system later.

## Architecture Overview

```
                    ConfigManager  (singleton, single entry point)
                          |
                    ConfigRegistry
        (thread-safe key-value store, std::unordered_map)
                          |
                    ConfigLoader
       (converts between text file and ConfigRegistry)
```

### Key design decisions

- **`ConfigValue` as `std::variant<bool, int64_t, double, std::string>`.**
  Type-safe storage for four value kinds without a manual type tag or
  `void*` casting.
- **Simple hand-rolled text format instead of JSON.** No third-party
  parsing library is linked by default (per the CMake "no third-party
  by default" rule), and a JSON parser is more complexity than the
  Engine currently needs. The format (`key = value`, `#`/`//` comments)
  is trivial to hand-edit and trivial to parse correctly.
- **`GetOrDefault<T>()` never throws or crashes on a missing key or a
  type mismatch.** If the key doesn't exist, or exists as a different
  type than requested, the caller's default is returned. This matches
  the reality that config files are hand-edited and will sometimes be
  missing keys or have typos.
- **Malformed individual lines are skipped, not fatal.** A config file
  with one bad line still loads every other valid line - this favors
  robustness over strict validation, matching how most game engines
  treat user-editable config files.

## Dependencies

None beyond the C++ standard library (`<variant>`, `<fstream>`,
`<unordered_map>`, `<mutex>`, `<optional>`). No dependency on Memory,
Logging, or Platform.

## Files Added

**Engine (7 files):**
- `Engine/Core/Config/ConfigTypes.h` *(header-only)*
- `Engine/Core/Config/ConfigRegistry.h` / `.cpp`
- `Engine/Core/Config/ConfigLoader.h` / `.cpp`
- `Engine/Core/Config/ConfigManager.h` / `.cpp`

**Tests (1 file):**
- `Tests/Unit/Core/ConfigTests.cpp`

**Integration (1 file):**
- `Integration/Scenarios/Core/ConfigIntegrationTests.cpp`

## Files Modified

- `Main.cpp` - added `ConfigManager::Get().Initialize()`/`Shutdown()`,
  and a `LoadFromFile("engine.cfg")` call with a graceful warning log
  when the file is absent. Init order is Memory, then Logging, then
  Config (so Config can log its own load result); shutdown order is
  Config, then Logging, then Memory.

## Public API Summary

| Call | Purpose |
|---|---|
| `ConfigManager::Get().Initialize()` / `Shutdown()` | Lifecycle |
| `ConfigManager::Get().LoadFromFile(path)` / `SaveToFile(path)` | Persistence |
| `ConfigManager::Get().Set(key, value)` | Write a value |
| `ConfigManager::Get().GetOrDefault<T>(key, defaultValue)` | Type-safe read with fallback |
| `ConfigManager::Get().Has(key)` | Existence check |
| `ConfigManager::Get().GetRegistry()` | Direct registry access (Remove, GetAllKeys, GetCount) |

## Unit Testing

**File:** `Tests/Unit/Core/ConfigTests.cpp`
**Result:** 8 / 8 passing

1. `Config_Manager_InitializeAndShutdown`
2. `Config_SetAndGet_ReturnsCorrectTypedValue`
3. `Config_GetOrDefault_ReturnsDefaultWhenMissing`
4. `Config_GetOrDefault_ReturnsDefaultWhenTypeMismatch`
5. `Config_Registry_HasAndRemove`
6. `Config_SaveAndLoad_RoundTrip`
7. `Config_Loader_SkipsCommentsAndBlankLines`
8. `Config_Loader_HandlesMissingFileGracefully`

## Integration Testing

**File:** `Integration/Scenarios/Core/ConfigIntegrationTests.cpp`
**Result:** 3 / 3 passing

1. `Integration_Config_LoadEngineSettingsAtStartup` - a realistic boot-time
   settings file is loaded and read by simulated subsystems
2. `Integration_Config_PersistUserSettingsAcrossRestarts` - simulates two
   Engine sessions: session 1 sets values and saves, session 2 loads and
   confirms the values survived
3. `Integration_Config_GracefulFallbackWhenConfigFileMissing` - simulates
   a brand-new install with no config file; Engine boots cleanly with
   defaults

Confirmed by user: all three CMake targets (`XR3D_Engine`,
`XR3D_UnitTests`, `XR3D_Integration`) built and ran clean together -
23/23 total unit tests (8 Config + 6 Logging + 9 Memory) and 10/10 total
integration scenarios (3 Config + 3 Logging + 4 Memory) passing in the
same run.

## Known Issues

None currently open.

## Resolved Issues

None - no defects found during this module's Build Validation or
Testing phases.

## Future Work (TODO markers left in code)

- Route file access through `Core/FileSystem` once that system exists,
  instead of `std::ifstream`/`std::ofstream` directly.
- Support hot-reload (watch the config file and re-load on change) once
  `Core/FileSystem`'s file-watcher exists.
- Add `ConfigValueType::Vector3`/`Color` once the Engine has shared math
  types to depend on.

## Git

| Field | Value |
|---|---|
| Commit | *(fill in after `git commit`)* |
| Tag | v0.4.0-config |

## Status

**Module = COMPLETE.** All pipeline steps finished except Git Commit/Tag/Push.