# Core / Logging Module

## Ye Module Kya Hai

Ye XR3D Engine ka **doosra Core system** hai. Poore Engine me kahin bhi kuch
print/log karna ho — Info message, Warning, Error — sab **isi Logging
module ke through** hoga. Koi bhi jagah seedha `printf`/`std::cout` use
nahi karegi; sab `Logger` se guzarke jaayega.

Isse fayda: ek hi jagah se decide ho jaata hai log kahan jaayega (console,
file, dono), kaunsa level dikhana hai, aur kis subsystem (category) ka log
chahiye — bina poore codebase me changes kiye.

---

## Folder Structure

```
Engine/Core/Logging/
│
├── README.md                  <- ye file
├── LogTypes.h                  (header-only - LogLevel/LogCategory/LogMessage)
├── LogFormatter.h/.cpp         (ILogFormatter interface + DefaultLogFormatter)
├── LogSink.h/.cpp              (ISink interface + ConsoleSink + FileSink)
├── LogRegistry.h/.cpp          (sinks manage karta hai, level/category filtering)
└── Logger.h/.cpp                (single entry point - sabka orchestrator)
```

**Related files doosri jagah:**

```
Tests/Unit/Core/LoggingTests.cpp                       -> 6 Unit Tests
Integration/Scenarios/Core/LoggingIntegrationTests.cpp -> 3 Integration Tests
Docs/DevelopmentLog/Core/Logging.md                    -> Development Log
Main.cpp                                               -> Initialize()/Shutdown() yahan judi hai
```

---

## Kaise Kaam Karta Hai (Architecture)

```
                    Logger  (singleton - sirf yehi bahar se access hoga)
                       |
                 LogRegistry
        (level filter + category filter, sinks ki list)
                       |
        -----------------------------------
        |                                 |
   ConsoleSink                        FileSink
  (stdout/stderr)                  (text file me likhta hai)
                       |
                 ILogFormatter
       (LogMessage ko final string me convert karta hai)
```

Har `Log()` call pehle `LogRegistry` se guzarta hai — wahan check hota hai
kya ye message ka level filter se upar hai, aur kya iski category enabled
hai. Dono pass hone par hi message format hoke sab registered sinks
(Console, File, ...) ko bheja jaata hai.

---

## Features

- **Single Entry Point** — poora Engine sirf `Logger::Get()` se baat karta hai
- **Type-Safe Formatting** — `std::format` use hota hai (`printf`-style `%d`/`%s` mismatch bugs nahi ho sakte)
- **Multiple Sinks Ek Saath** — Console aur File dono simultaneously chal sakte hain
- **Level Filtering** — Trace/Debug/Info/Warning/Error/Fatal, global minimum level set kar sakte ho
- **Category Filtering** — har category (Core/Rendering/Physics/Audio/...) alag se on/off kar sakte ho
- **Thread-Safe** — `LogRegistry` mutex se protected hai
- **File/Line Tracking** — macros automatically bata dete hain log kahan se aaya
- **Error/Fatal to stderr** — baaki sab `stdout` — console redirect/filter karna aasan
- **Koi Third-Party Dependency Nahi** — sirf standard C++ library

---

## Kaise Use Karein (Quick Example)

```cpp
#include "Core/Logging/Logger.h"

// Engine start hote waqt (Main.cpp me already jud chuka hai):
XR3D::Core::Logging::Logger::Get().Initialize();

// Kahin bhi log karna ho:
XR3D_LOG_INFO(XR3D::Core::Logging::LogCategory::Rendering, "Frame {} rendered in {} ms", frameIndex, ms);
XR3D_LOG_ERROR(XR3D::Core::Logging::LogCategory::Physics, "Solver diverged after {} iterations", n);

// File me bhi log chahiye ho to:
Logger::Get().GetRegistry().AddSink(std::make_unique<FileSink>("engine.log"));

// Engine band hote waqt:
XR3D::Core::Logging::Logger::Get().Shutdown();
```

---

## Testing Status

| Type | File | Result |
|---|---|---|
| Unit Tests | `Tests/Unit/Core/LoggingTests.cpp` | 6/6 Pass |
| Integration Tests | `Integration/Scenarios/Core/LoggingIntegrationTests.cpp` | 3/3 Pass |

---

## Git Tag

```
Tag Name : v0.3.0-logging
```

**Naming logic:** `v0.2.0` Memory ke liye use ho chuka tha, ye agla version
number hai. `-logging` suffix batata hai ye tag specifically Logging
module ki completion mark karta hai.

---

## Status

**COMPLETE** — Requirement se lekar Testing tak, poori pipeline khatam. Ab Documentation confirm + Git Commit/Tag/Push hi baaki hai.