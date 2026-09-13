# Core / Time Module

## Ye Module Kya Hai

Ye XR3D Engine ka **chautha Core system** hai. Poore Engine ki "ghadi"
(clock) yehi hai — frame delta-time, total elapsed time, pause/resume,
slow-motion/fast-forward, aur timers (cooldowns, delayed events) — sab
**isi Time module ke through** milega.

Ye sabse critical Core systems me se ek hai kyunki **Physics, Animation,
Rendering, Networking — sab isi par depend karenge.**

---

## Folder Structure

```
Engine/Core/Time/
│
├── README.md               <- ye file
├── TimeTypes.h               (header-only - Clock/TimePoint/Duration aliases, TimerType)
├── TimeHandle.h               (header-only - generational TimerHandle)
├── TimeTimer.h/.cpp           (ek single timer - Registry ke through hi use hota hai)
├── TimeRegistry.h/.cpp        (saare timers ka slot-map, safe create/destroy)
├── TimeClock.h/.cpp           (main frame clock - delta-time, scale, pause, fixed-step)
└── TimeManager.h/.cpp          (single entry point - sabka orchestrator)
```

**Related files doosri jagah:**

```
Tests/Unit/Core/TimeTests.cpp                       -> 10 Unit Tests
Integration/Scenarios/Core/TimeIntegrationTests.cpp -> 3 Integration Tests
Docs/DevelopmentLog/Core/Time.md                    -> Development Log
Main.cpp                                            -> Initialize()/Shutdown()/Tick() yahan judi hai
```

---

## Kaise Kaam Karta Hai (Architecture)

```
                    TimeManager  (singleton - sirf yehi bahar se access hoga)
                          |
              ------------------------
              |                      |
          TimeClock              TimeRegistry
   (delta-time, scale,        (sabhi active timers,
    pause, fixed-step)         generational handles ke saath)
                                     |
                                 TimeTimer
                            (ek single timer instance)
```

---

## Features

- **Single Entry Point** — poora Engine sirf `TimeManager::Get()` se baat karta hai
- **High-Resolution Delta-Time** — `std::chrono::steady_clock` use hota hai
- **Delta-Time Clamping** — agar ek frame bahut slow ho jaaye (breakpoint, disk stall), to agla delta bahut bada nahi hoga (max 250ms) — "spiral of death" se protection
- **Time Scale** — slow-motion ya fast-forward (`SetTimeScale(0.5)` = half speed)
- **Pause/Resume** — game-pause menu ke liye, delta-time turant 0 ho jaata hai
- **Fixed-Timestep Accumulator** — Physics ko variable frame-rate se decouple karta hai, deterministic behavior deta hai
- **Safe Timer Handles** — generation-counter ke saath, dangling-pointer bug possible hi nahi hai
- **One-Shot aur Repeating Timers** — cooldowns, delayed events, dono support hote hain
- **Deadlock-Safe Callbacks** — agar koi timer-callback khud naya timer banaye, to koi deadlock nahi hoga (callbacks lock release hone ke baad fire hote hain)
- **Multi-Fire Correctness** — bade delta (jaise slow frame) me repeating timer sahi baar fire hota hai, "beats" miss nahi hote
- **Koi Third-Party Dependency Nahi** — sirf standard C++ library

---

## Kaise Use Karein (Quick Example)

```cpp
#include "Core/Time/TimeManager.h"

XR3D::Core::Time::TimeManager::Get().Initialize();

// Har frame:
TimeManager::Get().Tick();
double dt = TimeManager::Get().GetDeltaTime();

// Fixed-timestep Physics ke liye:
while (TimeManager::Get().ConsumeFixedStep())
{
    // PhysicsStep(TimeManager::Get().GetFixedDeltaTime());
}

// Ek cooldown timer:
TimerHandle cooldown = TimeManager::Get().CreateTimer(2.0, TimerType::OneShot, []() {
    // ability ready ho gayi
});

// Slow-motion:
TimeManager::Get().SetTimeScale(0.5);

TimeManager::Get().Shutdown();
```

---

## Testing Status

| Type | File | Result |
|---|---|---|
| Unit Tests | `Tests/Unit/Core/TimeTests.cpp` | 10/10 Pass |
| Integration Tests | `Integration/Scenarios/Core/TimeIntegrationTests.cpp` | 3/3 Pass |

---

## Git Tag

```
Tag Name : v0.5.0-time
```

**Naming logic:** `v0.4.0` Config ke liye use ho chuka tha, ye agla version number hai. `-time` suffix batata hai ye tag specifically Time module ki completion mark karta hai.

---

## Status

**COMPLETE** — Requirement se lekar Testing tak, poori pipeline khatam. Ab Documentation confirm + Git Commit/Tag/Push hi baaki hai.