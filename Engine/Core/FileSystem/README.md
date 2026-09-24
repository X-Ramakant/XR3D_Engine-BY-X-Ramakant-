# Core / FileSystem Module

## Ye Module Kya Hai

Ye XR3D Engine ka **paanchwa Core system** hai. Poore Engine me kahin bhi
file padhni/likhni ho, folder banana ho, ya file-changes detect karni ho
— sab **isi FileSystem module ke through** hoga. Sabse important baat —
**is module ka koi bhi function kabhi exception `throw` nahi karta.**
Har operation ek safe result return karta hai, taaki missing file/folder
jaisi normal cheezein Engine ko kabhi crash na karein.

Ye module Logging ke `FileSink` aur Config ke `ConfigLoader` — dono ke
liye chhoड़े gaye TODO ko poora karega: dono abhi seedha `std::ifstream`/
`std::ofstream` use kar rahe the, ab unhe iske through migrate kiya
jaayega.

---

## Folder Structure

```
Engine/Core/FileSystem/
│
├── README.md                    <- ye file
├── FileSystemTypes.h              (header-only - FileError, FileResult<T>, FileInfo, WatchHandle)
├── FileSystemPath.h/.cpp          (path manipulation - std::filesystem par based, disk-access nahi)
├── FileSystemRegistry.h/.cpp      (virtual "alias://" mount points)
├── FileSystemWatcher.h/.cpp       (polling-based file-change detection)
└── FileSystemManager.h/.cpp        (single entry point - sabka orchestrator)
```

**Related files doosri jagah:**

```
Tests/Unit/Core/FileSystemTests.cpp                       -> 13 Unit Tests
Integration/Scenarios/Core/FileSystemIntegrationTests.cpp -> 3 Integration Tests
Docs/DevelopmentLog/Core/FileSystem.md                    -> Development Log
Main.cpp                                                  -> Initialize()/Shutdown() yahan judi hai
```

---

## Kaise Kaam Karta Hai (Architecture)

```
                    FileSystemManager  (singleton - sirf yehi bahar se access hoga)
                          |
              ------------------------------------
              |               |                   |
     FileSystemRegistry  FileSystemWatcher   FileSystemPath
    (alias:// mounts)    (polling-based       (pure string
                          file-watch)          manipulation)
                          |
                  std::filesystem (C++17 standard library)
```

---

## Features

- **Koi Exception Kahin Nahi** — har function `FileResult<T>` (success/error/value) ya `FileError` enum return karta hai, `throw` kabhi nahi
- **Text + Binary Dono** — `ReadAllText`/`WriteAllText`, `ReadAllBytes`/`WriteAllBytes`
- **Missing File/Folder = Graceful** — `NotFound` error milta hai, crash nahi
- **Cross-Platform Path Manipulation** — `std::filesystem` par based, `/` aur `\` dono handle hote hain
- **Virtual Mount Points** — `"assets://textures/wall.png"` jaisa clean path, asli disk-location alag se register hoti hai — project kahin bhi move ho, code nahi badalna padta
- **Unknown Alias = Graceful Pass-Through** — galat mount-alias use karne par bhi crash nahi, seedha `NotFound` milta hai
- **File Watching** — kisi bhi file ka change detect karo (`PollAll()` frame me bulao), safe generational `WatchHandle` ke saath
- **Deadlock-Safe Watcher Callbacks** — Time module jaisa hi pattern: agar koi watcher-callback khud naya watch banaye, koi deadlock nahi hoga
- **Directory Operations** — nested `CreateDirectory`, `ListDirectory`, `DeleteFile`
- **Koi Third-Party Dependency Nahi** — sirf standard C++17 `<filesystem>`

---

## Kaise Use Karein (Quick Example)

```cpp
#include "Core/FileSystem/FileSystemManager.h"

XR3D::Core::FileSystem::FileSystemManager::Get().Initialize();

// Text padhna (safe, crash nahi hoga agar file na ho):
auto result = FileSystemManager::Get().ReadAllText("config/settings.txt");
if (result.success)
{
    // result.value use karo
}
else
{
    // result.error check karo (jaise FileError::NotFound)
}

// Virtual mount:
FileSystemManager::Get().RegisterMount("assets", "D:/XR3D_Engine/Assets");
auto texture = FileSystemManager::Get().ReadAllBytes("assets://textures/wall.png");

// File watching (hot-reload ke liye):
auto handle = FileSystemManager::Get().Watch("config/settings.txt", []() {
    // file badla, reload karo
});
FileSystemManager::Get().PollWatches(); // har frame bulao

FileSystemManager::Get().Shutdown();
```

---

## Testing Status

| Type | File | Result |
|---|---|---|
| Unit Tests | `Tests/Unit/Core/FileSystemTests.cpp` | 13/13 Pass |
| Integration Tests | `Integration/Scenarios/Core/FileSystemIntegrationTests.cpp` | 3/3 Pass |

---

## Git Tag

```
Tag Name : v0.6.0-filesystem
```

**Naming logic:** `v0.5.0` Time ke liye use ho chuka tha, ye agla version
number hai. `-filesystem` suffix batata hai ye tag specifically
FileSystem module ki completion mark karta hai.

---

## Status

**COMPLETE** — Requirement se lekar Testing tak, poori pipeline khatam. Ab Documentation confirm + Git Commit/Tag/Push hi baaki hai.