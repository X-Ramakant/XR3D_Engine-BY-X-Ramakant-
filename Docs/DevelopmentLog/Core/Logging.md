# Core / Logging — Development Log

## Module Information

| Field | Value |
|---|---|
| **Module** | Logging |
| **Location** | `Engine/Core/Logging/` |
| **Main Module Group** | Core |
| **Version Phase** | V1 |
| **Internal Core Ordinal** | 2nd of 15 Core systems implemented |
| **Status** | COMPLETE |

## Purpose

Provides the single, unified entry point for all logging in the XR3D
Engine. Every subsystem writes through `Logger` rather than printing
directly, so output destinations (console, file, ...), verbosity level,
and per-subsystem visibility can be controlled from one place.

## Why Required

Logging is the second Core system built (right after Memory) because
every other system built from here on will need to report status, errors
and diagnostics as it is implemented and tested. Building it now means
Config, Time, FileSystem and everything after can log from day one
instead of relying on ad-hoc `printf` calls that would later need to be
replaced.

## Architecture Overview

```
                    Logger  (singleton, single entry point)
                       |
                 LogRegistry
        (level filter + category filter, owns sinks)
                       |
        -----------------------------------
        |                                 |
   ConsoleSink                        FileSink
                       |
                 ILogFormatter
              (DefaultLogFormatter)
```

### Key design decisions

- **`std::format` instead of `printf`.** Type-safe variadic formatting via
  `XR3D_LOG_*` macros - eliminates an entire class of format-string bugs.
- **Sink abstraction (`ISink`).** `ConsoleSink` and `FileSink` are both
  swappable/addable at runtime; a `PlatformDebugSink` (VS Output window)
  can be added later without touching call sites.
- **Level and category filtering live in `LogRegistry`, not `Logger`.**
  Keeps `Logger` itself a thin dispatcher; filtering logic is testable in
  isolation (see Unit Tests).
- **Error/Fatal route to `stderr`**, everything else to `stdout` - makes
  console redirection and CI log parsing straightforward.

## Dependencies

None beyond the C++ standard library (`<format>`, `<fstream>`, `<mutex>`,
`<chrono>`). No dependency on Memory or Platform - Logging must be usable
extremely early in Engine boot, before more complex systems exist.

## Files Added

**Engine (9 files):**
- `Engine/Core/Logging/LogTypes.h` *(header-only)*
- `Engine/Core/Logging/LogFormatter.h` / `.cpp`
- `Engine/Core/Logging/LogSink.h` / `.cpp`
- `Engine/Core/Logging/LogRegistry.h` / `.cpp`
- `Engine/Core/Logging/Logger.h` / `.cpp`

**Tests (1 file):**
- `Tests/Unit/Core/LoggingTests.cpp`

**Integration (1 file):**
- `Integration/Scenarios/Core/LoggingIntegrationTests.cpp`

## Files Modified

- `Main.cpp` - added `Logger::Get().Initialize()`/`Shutdown()`, replaced
  `printf` calls with `XR3D_LOG_INFO`. Init order is Memory then Logging;
  shutdown order is Logging then Memory (reverse), so Logging can flush
  its last messages before Memory tears down.

## Public API Summary

| Call | Purpose |
|---|---|
| `Logger::Get().Initialize()` / `Shutdown()` | Lifecycle |
| `XR3D_LOG_TRACE/DEBUG/INFO/WARNING/ERROR/FATAL(category, fmt, ...)` | Type-safe logging macros |
| `Logger::Get().GetRegistry().AddSink(...)` | Add a sink (Console/File/custom) |
| `Logger::Get().GetRegistry().SetMinLevel(level)` | Global verbosity control |
| `Logger::Get().GetRegistry().SetCategoryEnabled(category, bool)` | Per-subsystem on/off |

## Unit Testing

**File:** `Tests/Unit/Core/LoggingTests.cpp`
**Result:** 6 / 6 passing

1. `Logging_Logger_InitializeAndShutdown`
2. `Logging_DefaultFormatter_ContainsLevelCategoryAndText`
3. `Logging_FileSink_WritesToFile`
4. `Logging_Registry_CategoryFilter_BlocksDisabledCategory`
5. `Logging_Registry_LevelFilter_BlocksBelowMinLevel`
6. `Logging_Macro_FormatsArgumentsCorrectly`

## Integration Testing

**File:** `Integration/Scenarios/Core/LoggingIntegrationTests.cpp`
**Result:** 3 / 3 passing

1. `Integration_Logging_ConsoleAndFileSinkTogether` - both sinks active simultaneously
2. `Integration_Logging_SimulatedFrameLoop_HighVolume` - 200 simulated frames of Trace-level logging across two categories
3. `Integration_Logging_RuntimeLevelAndCategoryToggling` - verbosity changed mid-run without restart

Confirmed by user: all three CMake targets (`XR3D_Engine`, `XR3D_UnitTests`,
`XR3D_Integration`) built and ran clean together - 15/15 total unit tests
(6 Logging + 9 Memory) and 7/7 total integration scenarios (3 Logging + 4
Memory) passing in the same run.

## Known Issues

None currently open.

## Resolved Issues

None - no defects found during this module's Build Validation or Testing
phases.

## Future Work (TODO markers left in code)

- Route Fatal-level logs through `Core/Diagnostics` (e.g. trigger a crash
  report) once that system exists.
- `FileSink` currently opens `std::ofstream` directly; migrate to
  `Core/FileSystem` once that system exists.
- Add a `PlatformDebugSink` (Visual Studio Output window) once
  `Platform/Windows` exists.
- Add a `JsonLogFormatter` if external log aggregation or crash-reporting
  tooling later needs structured output instead of plain text.
- Per-sink minimum level (e.g. Console shows Info+, File captures
  everything) if a real use case needs it.

## Git

| Field | Value |
|---|---|
| Commit | *(fill in after `git commit`)* |
| Tag | v0.3.0-logging |

## Status

**Module = COMPLETE.** All pipeline steps finished except Git Commit/Tag/Push.