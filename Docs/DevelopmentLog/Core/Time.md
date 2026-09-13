# Core / Time — Development Log

## Module Information

| Field | Value |
|---|---|
| **Module** | Time |
| **Location** | `Engine/Core/Time/` |
| **Main Module Group** | Core |
| **Version Phase** | V1 |
| **Internal Core Ordinal** | 4th of 15 Core systems implemented |
| **Status** | COMPLETE |

## Purpose

Provides the single, unified entry point for all engine timing: the
frame clock (delta time, total elapsed time), time-scale (slow-motion/
fast-forward), pause/resume, a fixed-timestep accumulator for
deterministic Physics-style updates, and a safe timer system for
cooldowns and delayed/repeating events.

## Why Required

Time is the fourth Core system built. Nearly every future subsystem -
Physics (fixed-step simulation), Animation (blending over time),
Rendering (frame pacing), Networking (interpolation/timeouts), and
gameplay logic (cooldowns, buffs) - depends on a single, correct,
authoritative source of "what time is it and how much has passed".
Building this early, and building it complete (time-scale, pause,
fixed-step, safe timers all at once) avoids retrofitting these features
into systems that assume a bare `deltaTime` later.

## Architecture Overview

```
                    TimeManager  (singleton, single entry point)
                          |
              ------------------------
              |                      |
          TimeClock              TimeRegistry
   (delta-time, scale,        (all active timers,
    pause, fixed-step)         generational handles)
                                     |
                                 TimeTimer
                            (a single timer instance)
```

### Key design decisions

- **Delta-time clamping (`SetMaxDeltaTime`, default 0.25s).** Protects
  against the "spiral of death": if a single frame takes an unusually
  long real-world time (breakpoint, disk stall, OS hiccup), the next
  `Tick()` would otherwise report a huge delta, causing
  Physics/Animation to take one giant, unstable step. The clamp caps
  the *reported* delta while real time keeps passing normally.
- **Fixed-timestep accumulator, built in from the start.** Rather than
  bolting deterministic stepping onto Physics later, `TimeClock` itself
  accumulates scaled delta time and exposes `ConsumeFixedStep()`, so any
  future system (Physics, networked simulation) can drive itself at a
  constant rate regardless of variable frame rate.
- **Generational `TimerHandle`, not raw pointers.** A destroyed timer's
  slot is recycled by a later `CreateTimer()` call. A generation counter
  on each slot means a stale handle referring to a reused index is
  reliably detected as invalid, rather than silently referring to an
  unrelated timer (a classic dangling-reference class of bug).
- **Deferred callback firing (deadlock/reentrancy safety).** A timer's
  `Advance()` never invokes its own callback. `TimeRegistry::UpdateAll()`
  collects all due callbacks while holding its mutex, then releases the
  lock *before* firing any of them. This means a callback that itself
  creates or destroys other timers (a realistic gameplay pattern - e.g.
  a cooldown finishing and immediately starting a follow-up effect)
  cannot deadlock on the same non-recursive mutex, and cannot corrupt
  the registry mid-iteration.
- **Multi-fire correctness for repeating timers.** `TimeTimer::Advance()`
  returns a fire *count*, not just a boolean - if a single large delta
  (e.g. after a frame stall) spans more than one full interval of a
  repeating timer, it fires once per interval crossed instead of
  silently dropping "beats". This was caught by an early sandbox test
  and fixed before delivery (see *Resolved Issues*).

## Dependencies

None beyond the C++ standard library (`<chrono>`, `<functional>`,
`<mutex>`, `<vector>`). No dependency on Memory, Logging, Config, or
Platform.

## Files Added

**Engine (10 files):**
- `Engine/Core/Time/TimeTypes.h` *(header-only)*
- `Engine/Core/Time/TimeHandle.h` *(header-only)*
- `Engine/Core/Time/TimeTimer.h` / `.cpp`
- `Engine/Core/Time/TimeRegistry.h` / `.cpp`
- `Engine/Core/Time/TimeClock.h` / `.cpp`
- `Engine/Core/Time/TimeManager.h` / `.cpp`

**Tests (1 file):**
- `Tests/Unit/Core/TimeTests.cpp`

**Integration (1 file):**
- `Integration/Scenarios/Core/TimeIntegrationTests.cpp`

## Files Modified

- `Main.cpp` - added `TimeManager::Get().Initialize()`/`Shutdown()`, and
  a demonstration `Tick()` call logging the first frame's delta time.
  Init order is Memory, Logging, Config, then Time; shutdown order is
  the exact reverse (Time, Config, Logging, Memory).

## Public API Summary

| Call | Purpose |
|---|---|
| `TimeManager::Get().Initialize()` / `Shutdown()` | Lifecycle |
| `TimeManager::Get().Tick()` | Advance the clock one frame; call once per frame |
| `GetDeltaTime()` / `GetUnscaledDeltaTime()` / `GetTotalTime()` | Frame timing queries |
| `SetTimeScale(scale)` / `GetTimeScale()` | Slow-motion / fast-forward |
| `Pause()` / `Resume()` / `IsPaused()` | Pause state |
| `SetMaxDeltaTime(seconds)` | Spiral-of-death protection tuning |
| `ConsumeFixedStep()` / `GetFixedDeltaTime()` / `SetFixedDeltaTime()` | Fixed-timestep accumulator for Physics-style updates |
| `CreateTimer(duration, type, callback)` / `DestroyTimer(handle)` / `IsTimerValid(handle)` | Timer lifecycle |

## Unit Testing

**File:** `Tests/Unit/Core/TimeTests.cpp`
**Result:** 10 / 10 passing

1. `Time_Manager_InitializeAndShutdown`
2. `Time_Clock_DeltaTimeIsPositiveAfterTick`
3. `Time_Clock_PauseSetsZeroDelta`
4. `Time_Clock_TimeScaleAffectsDelta`
5. `Time_Clock_MaxDeltaTimeClamps`
6. `Time_Timer_OneShot_FiresOnceAndCleansUp`
7. `Time_Timer_Repeating_FiresMultipleTimesForLargeDelta`
8. `Time_Timer_StaleHandle_DetectedAfterReuse`
9. `Time_FixedTimestep_ConsumeFixedStepWorks`
10. `Time_Timer_ReentrantCallback_DoesNotDeadlock`

Note: because `TimeManager` is a process-wide singleton, two tests
(`Time_Clock_MaxDeltaTimeClamps` and `Time_FixedTimestep_ConsumeFixedStepWorks`)
explicitly reset/set `SetMaxDeltaTime()` rather than relying on the
default, since a setting changed in one test would otherwise leak into
a later test running in the same process. This is a test-isolation
concern, not a defect in `TimeClock` itself.

## Integration Testing

**File:** `Integration/Scenarios/Core/TimeIntegrationTests.cpp`
**Result:** 3 / 3 passing

1. `Integration_Time_SimulatedGameLoop_FixedStepDrivesPhysics` - 10
   simulated ~60 FPS frames driving a fixed-timestep accumulator, the
   way a real Physics update loop will once Physics exists
2. `Integration_Time_AbilityCooldownSystem` - three concurrent one-shot
   timers with different durations, advanced via small repeated ticks
   (the way a real per-frame cooldown UI would update)
3. `Integration_Time_PauseMenuScenario` - a repeating timer is confirmed
   to NOT advance while `Pause()` is active, and resumes firing
   correctly after `Resume()`

Confirmed by user: all three CMake targets built and ran clean together
- 33/33 total unit tests (10 Time + 8 Config + 6 Logging + 9 Memory) and
13/13 total integration scenarios (3 Time + 3 Config + 3 Logging + 4
Memory) passing in the same run.

## Known Issues

None currently open.

## Resolved Issues

- **Repeating timer under-firing on large delta.** The first
  implementation of `TimeTimer::Advance()` only checked the duration
  crossing once per call, so a delta spanning multiple full intervals
  (e.g. `UpdateAll(0.35)` on a `0.1`s repeating timer) fired only once
  instead of three times, silently dropping "beats". Caught by a
  sandbox test before delivery; fixed by changing `Advance()` to return
  a fire *count* computed via an internal loop, with `TimeRegistry`
  pushing that many callback copies to fire.
- **Test-isolation state leak (not a module defect).** An early version
  of the flaky-appearing integration test (`Integration_Time_
  SimulatedGameLoop_FixedStepDrivesPhysics`) used a tight upper bound
  (`<= 12`) on expected fixed-step count derived from `sleep_for()`
  timing, which is not precise under OS scheduler jitter (especially
  in Debug builds, and especially right after a 200-line-output
  Logging test in the same process). Widened to a looser, still
  meaningful bound (`>= 5 && <= 20`) - confirmed passing afterward.

## Future Work (TODO markers left in code)

- Make `maxDeltaTime`/`fixedDeltaTime` configurable via `Core/Config`
  once that integration point is wired up in `TimeClock`.
- Add a `TimeUnit` enum (Seconds/Milliseconds/Frames) if a formatting/
  conversion utility is ever needed.
- Switch `TimeRegistry`'s linear `UpdateAll()` scan to a priority queue
  / sorted structure if the number of concurrent timers grows large
  enough that this becomes a measured bottleneck.

## Git

| Field | Value |
|---|---|
| Commit | *(fill in after `git commit`)* |
| Tag | v0.5.0-time |

## Status

**Module = COMPLETE.** All pipeline steps finished except Git Commit/Tag/Push.