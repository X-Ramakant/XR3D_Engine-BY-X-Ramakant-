
All five allocator strategies implement the common `IAllocator` interface
(`Allocate`, `Deallocate`, `Reset`, `GetName`, `GetType`), so `MemoryManager`
can route calls polymorphically and new strategies can be added later
without touching call sites.

### Key design decisions

- **`MemoryManager::Deallocate(pointer)` looks up the owning allocator
  before freeing.** The allocator type used for each allocation is
  recorded in `MemoryTracker`, so a generic `Deallocate()` call always
  routes to the correct strategy (Default/Linear/Stack/Pool/FreeList)
  instead of assuming Default. This was a bug found during Build
  Validation and fixed before Unit Testing (see *Resolved Issues*).
- **Tag-based accounting.** Every allocation carries a `MemoryTag`
  (Rendering, Physics, Audio, ...) so per-subsystem memory usage can be
  reported independently of global totals — useful once Rendering (V4)
  and Physics (V7) start allocating heavily.
- **No forced `.cpp` for pure data.** `MemoryTypes.h` is header-only
  (enums/structs only), per the project-wide header-only rule.

## Dependencies

None. Memory is implemented using only the C++ standard library
(`<cstdlib>`, `<mutex>`, `<unordered_map>`, `<memory>`, `<vector>`) and has
no dependency on Platform or any other Core system — intentional, since
it must be the very first system available during Engine boot.

## Files Added

**Engine (9 files):**
- `Engine/Core/Memory/MemoryTypes.h` *(header-only)*
- `Engine/Core/Memory/MemoryAllocator.h` / `.cpp`
- `Engine/Core/Memory/MemoryPool.h` / `.cpp`
- `Engine/Core/Memory/MemoryTracker.h` / `.cpp`
- `Engine/Core/Memory/MemoryManager.h` / `.cpp`

**Tests (3 files):**
- `Tests/Unit/TestFramework.h`
- `Tests/Unit/main_tests.cpp`
- `Tests/Unit/Core/MemoryTests.cpp`

**Integration (3 files):**
- `Integration/IntegrationTestFramework.h`
- `Integration/main_integration.cpp`
- `Integration/Scenarios/Core/MemoryIntegrationTests.cpp`

**Build system (2 files, first written for this module):**
- `Tests/CMakeLists.txt` — defines `XR3D_UnitTests`
- `Integration/CMakeLists.txt` — defines `XR3D_Integration`

## Files Modified

- `Main.cpp` — wired `MemoryManager::Initialize()` at startup and
  `MemoryManager::Shutdown()` at shutdown (Engine Integration step).

## Public API Summary

| Call | Purpose |
|---|---|
| `MemoryManager::Get().Initialize()` / `Shutdown()` | Lifecycle |
| `XR3D_ALLOC(size, tag)` / `XR3D_FREE(ptr)` | Convenience macros, capture file/line automatically |
| `MemoryManager::Get().Allocate(size, allocatorType, tag, file, line)` | Explicit allocator selection |
| `MemoryManager::Get().GetAllocator(type)` | Direct access (e.g. `StackAllocator` markers) |
| `MemoryManager::Get().CreatePool(blockSize, blockCount)` | Create a dedicated fixed-block pool |
| `MemoryManager::Get().GetGlobalStats()` / `GetStatsForTag(tag)` | Reporting |
| `MemoryManager::Get().DumpReport()` | Prints usage + any active leaks |

## Unit Testing

**File:** `Tests/Unit/Core/MemoryTests.cpp`
**Result:** 9 / 9 passing

1. `Memory_Manager_InitializeAndShutdown`
2. `Memory_DefaultAllocator_AllocateAndFree`
3. `Memory_PerTagStats_TrackSeparately`
4. `Memory_LinearAllocator_BumpAndReset`
5. `Memory_StackAllocator_MarkerRewind`
6. `Memory_Pool_AllocateFreeAndBlockCount`
7. `Memory_FreeListAllocator_AllocateAndFree`
8. `Memory_Deallocate_RoutesToCorrectAllocator`
9. `Memory_NoLeaksAfterShutdown`

## Integration Testing

**File:** `Integration/Scenarios/Core/MemoryIntegrationTests.cpp`
**Result:** 4 / 4 passing

1. `Integration_Memory_SimulatedFrameLoop_NoLeaks` — 60 simulated frames of Linear-allocator scratch usage
2. `Integration_Memory_MultiSystem_TaggedAllocations_NoLeaks` — Rendering/Physics/Audio allocating concurrently
3. `Integration_Memory_StackAllocator_NestedScopes` — nested marker push/rewind
4. `Integration_Memory_Pool_ExhaustionHandledGracefully` — pool exhaustion returns `nullptr`, does not crash

## Known Issues

None currently open.

## Resolved Issues

- **Null-pointer dereference on allocation failure.** `LinearAllocator`,
  `StackAllocator`, `FreeListAllocator` and `MemoryPool` all dereferenced
  their backing buffer immediately after `malloc`, without checking for
  failure. Fixed by adding `assert` guards and null-checks in
  `MemoryAllocator.cpp` and `MemoryPool.cpp` before Unit Testing began.

## Future Work (TODO markers left in code)

- Configurable arena sizes for Linear/Stack/FreeList via `Core/Config`
  (once that system exists), instead of the current fixed constants.
- Best-fit search and adjacent free-block coalescing in `FreeListAllocator`.
- Route allocation-failure reporting through `Core/Diagnostics` once that
  system exists, instead of `assert`-only.
- NUMA-node / GPU-visible memory tagging once Rendering (V4) needs
  device-local allocations.
- Growable `MemoryPool` support (chain additional blocks once exhausted).

## Git

| Field | Value |
|---|---|
| Commit | *(fill in after `git commit`)* |
| Tag | *(fill in after `git tag`)* |

## Status

**Module = COMPLETE.** All pipeline steps finished except Git Commit/Tag/Push, which follow immediately after this document is saved.