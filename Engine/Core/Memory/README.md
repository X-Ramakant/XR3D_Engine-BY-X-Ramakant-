# Core / Memory Module

## Ye Module Kya Hai

Ye XR3D Engine ka **sabse pehla Core system** hai. Poore Engine me kahin bhi
memory allocate/free karni ho — Logging ho, Config ho, Rendering ho, Physics
ho — sab **isi Memory module ke through** hoga. Koi bhi jagah seedha
`new`/`malloc` use nahi karegi; sab `MemoryManager` se guzarke jaayega.

Isse fayda: kitni memory kaha use ho rahi hai, kahin leak to nahi ho raha,
sab jagah se **ek hi jagah** track ho jaata hai.

---

## Folder Structure

```
Engine/Core/Memory/
│
├── README.md                  <- ye file
├── MemoryTypes.h               (header-only - Types/Enums/Stats)
├── MemoryAllocator.h/.cpp      (IAllocator interface + 4 strategies)
├── MemoryPool.h/.cpp           (5th strategy - fixed block pool)
├── MemoryTracker.h/.cpp        (allocation tracking + leak detection)
└── MemoryManager.h/.cpp        (single entry point - sabka orchestrator)
```

**Related files doosri jagah:**

```
Tests/Unit/Core/MemoryTests.cpp                       -> 9 Unit Tests
Integration/Scenarios/Core/MemoryIntegrationTests.cpp -> 4 Integration Tests
Docs/DevelopmentLog/Core/Memory.md                    -> Development Log
Main.cpp                                              -> Initialize()/Shutdown() yahan judi hai
```

---

## Kaise Kaam Karta Hai (Architecture)

```
                    MemoryManager  (singleton - sirf yehi bahar se access hoga)
                          |
        ---------------------------------------------------
        |          |            |             |            |
   Default     Linear        Stack        FreeList      Pool
  Allocator   Allocator     Allocator     Allocator    (jitne bhi
  (malloc-    (bump, per-   (LIFO,        (kabhi bhi     banao,
   based)      frame use)    marker/       order me      alag-alag
                             rewind)       free ho          size ke)
                                           sakta hai)
                          |
                    MemoryTracker
        (har allocation record karta hai, leak dhoondhta hai,
              Tag ke hisaab se stats deta hai)
```

Sabhi 5 allocators ek hi common interface (`IAllocator`) follow karte hain —
isliye `MemoryManager` ko pata nahi hona chahiye ki andar konsa allocator
chal raha hai, wo sirf ek jaisa call karta hai.

---

## 5 Allocator Strategies (kya-kaam-kab-use-hoga)

| Allocator | Kaise Kaam Karta Hai | Kab Use Hoga |
|---|---|---|
| **Default** | Seedha OS heap (`malloc`/`free`) | Jab koi special zaroorat na ho — general purpose |
| **Linear** | Ek buffer me lagatar aage badhta jaata hai, individual free nahi hota, sirf `Reset()` se poora khaali | Per-frame scratch memory (jaisa game loop me har frame ka temporary data) |
| **Stack** | LIFO — jo aakhri me allocate hua wahi pehle free hoga, "marker" rakhke ek jagah tak wapas ja sakte hain | Nested function calls me scratch memory (ek function ke andar doosra) |
| **FreeList** | Kabhi bhi order me allocate/free ho sakta hai, free blocks ki list maintain karta hai | General purpose jab baar-baar variable-size allocate/free karna ho |
| **Pool** | Sab blocks ek hi fixed size ke — bahut fast allocate/free | Same-type cheezein baar-baar banani ho (jaise ECS Components, chhote objects) |

---

## Features

- **Single Entry Point** — poora Engine sirf `MemoryManager::Get()` se baat karta hai
- **5 Allocation Strategies** — Default, Linear, Stack, Pool, FreeList
- **Automatic Correct Routing** — `Deallocate()` khud pata laga leta hai kis allocator se allocate hua tha, wahi use karke free karta hai
- **Tag-based Tracking** — har allocation ek Tag (Core/Rendering/Physics/Audio/...) ke saath track hota hai, alag-alag subsystem ki memory usage alag dikh sakti hai
- **Leak Detection** — Engine shutdown hote waqt agar koi memory free hone se reh gayi, to report print hoga (file+line ke saath)
- **Thread-Safe Tracking** — `MemoryTracker` mutex se protected hai
- **File/Line Tracking** — `XR3D_ALLOC`/`XR3D_FREE` macros automatically bata dete hain allocation kahan se hui
- **Null-Safety** — agar `malloc` fail ho jaaye (bahut badi memory maange), crash nahi hota, safe assert/guard hai
- **Koi Third-Party Dependency Nahi** — sirf standard C++ library

---

## Kaise Use Karein (Quick Example)

```cpp
#include "Core/Memory/MemoryManager.h"

// Engine start hote waqt (Main.cpp me already jud chuka hai):
XR3D::Core::Memory::MemoryManager::Get().Initialize();

// Kahin bhi allocate karna ho:
void* data = XR3D_ALLOC(256, XR3D::Core::Memory::MemoryTag::Rendering);

// Free karna:
XR3D_FREE(data);

// Engine band hote waqt:
XR3D::Core::Memory::MemoryManager::Get().Shutdown();
```

---

## Testing Status

| Type | File | Result |
|---|---|---|
| Unit Tests | `Tests/Unit/Core/MemoryTests.cpp` | 9/9 Pass |
| Integration Tests | `Integration/Scenarios/Core/MemoryIntegrationTests.cpp` | 4/4 Pass |

---

## Git Tag

```
Tag Name : v0.2.0-memory
```

**Naming logic:** `v0.2.0` — kyunki `v0.1.0` pehle se "Project Setup" (foundation/skeleton) ke liye use ho chuka hai, ye agla version number hai. `-memory` suffix batata hai ye tag specifically Memory module ki completion mark karta hai (aapki apni versioning convention: `vMajor.Minor.Patch-Module`).

---

## Status

**COMPLETE** — Requirement se lekar Documentation tak, poori pipeline khatam. Ab Git Commit/Tag/Push hi baaki hai.