# Core / Config Module

## Ye Module Kya Hai

Ye XR3D Engine ka **teesra Core system** hai. Poore Engine me kahin bhi
koi setting/configuration chahiye ho — window size, fullscreen flag,
audio volume, physics gravity — sab **isi Config module ke through**
milegi. Koi bhi jagah seedha hardcoded values nahi likhega; sab
`ConfigManager` se guzarke aayega.

Isse fayda: settings ek text file me rakhi ja sakti hain, engine restart
karne par player ki pasand (settings) yaad rehti hain, aur naye
developers ko settings dhoondhne ke liye code me khoj karne ki zaroorat
nahi padti — sab ek jagah.

---

## Folder Structure

```
Engine/Core/Config/
│
├── README.md                  <- ye file
├── ConfigTypes.h                (header-only - ConfigValue variant, ConfigValueType)
├── ConfigRegistry.h/.cpp        (thread-safe key-value store)
├── ConfigLoader.h/.cpp          (text file se load/save karta hai)
└── ConfigManager.h/.cpp          (single entry point - sabka orchestrator)
```

**Related files doosri jagah:**

```
Tests/Unit/Core/ConfigTests.cpp                       -> 8 Unit Tests
Integration/Scenarios/Core/ConfigIntegrationTests.cpp -> 3 Integration Tests
Docs/DevelopmentLog/Core/Config.md                    -> Development Log
Main.cpp                                              -> Initialize()/Shutdown() yahan judi hai
```

---

## Kaise Kaam Karta Hai (Architecture)

```
                    ConfigManager  (singleton - sirf yehi bahar se access hoga)
                          |
                    ConfigRegistry
        (thread-safe key-value store - std::unordered_map)
                          |
                    ConfigLoader
       (text file <-> ConfigRegistry ke beech convert karta hai)
```

`ConfigValue` ek `std::variant<bool, int64_t, double, std::string>`
hai — matlab ek hi value type-safe tarike se 4 me se koi bhi type ho
sakti hai, bina manual tag maintain kiye.

---

## File Format

Config file ek simple text format me hoti hai:

```
# ye comment hai
// ye bhi comment hai

window.width = 1920
window.height = 1080
window.fullscreen = false
audio.masterVolume = 0.8
player.name = "Ramakant"
```

- `#` ya `//` se shuru hone wali line comment maani jaati hai
- khali lines ignore hoti hain
- `key = value` format — value automatically type detect hoti hai
  (`true`/`false` -> bool, sirf digits -> int, decimal point wale
  digits -> double, baaki sab -> string, quotes optional hain)
- galat-format wali line (jisme `=` na ho) sirf skip ho jaati hai,
  poori file load fail nahi hoti

---

## Features

- **Single Entry Point** — poora Engine sirf `ConfigManager::Get()` se baat karta hai
- **Type-Safe Values** — `std::variant` use hota hai, koi manual casting nahi
- **Thread-Safe** — `ConfigRegistry` mutex se protected hai
- **Graceful Fallback** — file na milne par crash nahi hota, `GetOrDefault<T>()` se default value milti hai
- **Type-Mismatch Safety** — agar wrong type maanga jaaye (jaise int ko bool samajhkar), to crash nahi, default value milti hai
- **Save + Load Round-Trip** — settings ko file me save karke, baad me wapas load kiya ja sakta hai (player settings persist karne ke liye)
- **Comments Support** — config file me `#` ya `//` se comments likh sakte hain
- **Koi Third-Party Dependency Nahi** — apna khud ka chhota parser, koi JSON library nahi

---

## Kaise Use Karein (Quick Example)

```cpp
#include "Core/Config/ConfigManager.h"

// Engine start hote waqt (Main.cpp me already jud chuka hai):
XR3D::Core::Config::ConfigManager::Get().Initialize();

// File se load karna:
if (!ConfigManager::Get().LoadFromFile("engine.cfg"))
{
    // file nahi mili - defaults use honge, koi problem nahi
}

// Value padhna (type-safe, default ke saath):
int64_t width = ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);

// Value set karna:
ConfigManager::Get().Set("audio.masterVolume", ConfigValue{ 0.8 });

// File me save karna:
ConfigManager::Get().SaveToFile("engine.cfg");

// Engine band hote waqt:
ConfigManager::Get().Shutdown();
```

---

## Testing Status

| Type | File | Result |
|---|---|---|
| Unit Tests | `Tests/Unit/Core/ConfigTests.cpp` | 8/8 Pass |
| Integration Tests | `Integration/Scenarios/Core/ConfigIntegrationTests.cpp` | 3/3 Pass |

---

## Git Tag

```
Tag Name : v0.4.0-config
```

**Naming logic:** `v0.3.0` Logging ke liye use ho chuka tha, ye agla
version number hai. `-config` suffix batata hai ye tag specifically
Config module ki completion mark karta hai.

---

## Status

**COMPLETE** — Requirement se lekar Testing tak, poori pipeline khatam. Ab Documentation confirm + Git Commit/Tag/Push hi baaki hai.