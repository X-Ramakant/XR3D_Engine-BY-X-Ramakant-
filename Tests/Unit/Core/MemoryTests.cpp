/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Unit tests for the Core/Memory subsystem: MemoryManager, and the
* Default, Linear, Stack, Pool and FreeList allocator strategies.

*/

#include "TestFramework.h"
#include "Core\Memory\MemoryManager.h"

using namespace XR3D::Core::Memory;

XR3D_TEST(Memory_Manager_InitializeAndShutdown)
{
    MemoryManager::Get().Initialize();
    MemoryManager::Get().Initialize(); // second call must be a safe no-op
    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_DefaultAllocator_AllocateAndFree)
{
    MemoryManager::Get().Initialize();

    void* p = XR3D_ALLOC(128, MemoryTag::Core);
    XR3D_CHECK_NOT_NULL(p);

    MemoryStats stats = MemoryManager::Get().GetGlobalStats();
    XR3D_CHECK_EQ(stats.currentUsage, 128u);
    XR3D_CHECK_EQ(stats.allocationCount, 1u);

    XR3D_FREE(p);

    stats = MemoryManager::Get().GetGlobalStats();
    XR3D_CHECK_EQ(stats.currentUsage, 0u);
    XR3D_CHECK_EQ(stats.deallocationCount, 1u);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_PerTagStats_TrackSeparately)
{
    MemoryManager::Get().Initialize();

    void* p1 = MemoryManager::Get().Allocate(64, AllocatorType::Default, MemoryTag::Rendering, __FILE__, __LINE__);
    void* p2 = MemoryManager::Get().Allocate(32, AllocatorType::Default, MemoryTag::Physics, __FILE__, __LINE__);

    MemoryStats renderStats = MemoryManager::Get().GetStatsForTag(MemoryTag::Rendering);
    MemoryStats physicsStats = MemoryManager::Get().GetStatsForTag(MemoryTag::Physics);

    XR3D_CHECK_EQ(renderStats.currentUsage, 64u);
    XR3D_CHECK_EQ(physicsStats.currentUsage, 32u);

    MemoryManager::Get().Deallocate(p1);
    MemoryManager::Get().Deallocate(p2);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_LinearAllocator_BumpAndReset)
{
    MemoryManager::Get().Initialize();

    auto& linear = static_cast<LinearAllocator&>(MemoryManager::Get().GetAllocator(AllocatorType::Linear));

    void* a = linear.Allocate(64);
    void* b = linear.Allocate(64);
    XR3D_CHECK_NOT_NULL(a);
    XR3D_CHECK_NOT_NULL(b);
    XR3D_CHECK_NE(a, b);

    linear.Reset();
    void* c = linear.Allocate(64);
    XR3D_CHECK_EQ(a, c); // after Reset, bump pointer starts over

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_StackAllocator_MarkerRewind)
{
    MemoryManager::Get().Initialize();

    auto& stack = static_cast<StackAllocator&>(MemoryManager::Get().GetAllocator(AllocatorType::Stack));

    StackAllocator::Marker marker = stack.GetMarker();
    void* a = stack.Allocate(64);
    XR3D_CHECK_NOT_NULL(a);

    stack.RewindToMarker(marker);
    void* b = stack.Allocate(64);
    XR3D_CHECK_EQ(a, b); // after rewind, same offset is reused

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_Pool_AllocateFreeAndBlockCount)
{
    MemoryManager::Get().Initialize();

    MemoryPool& pool = MemoryManager::Get().CreatePool(sizeof(int), 4);
    XR3D_CHECK_EQ(pool.GetFreeBlockCount(), 4u);

    void* a = pool.Allocate(sizeof(int));
    void* b = pool.Allocate(sizeof(int));
    XR3D_CHECK_NOT_NULL(a);
    XR3D_CHECK_NOT_NULL(b);
    XR3D_CHECK_EQ(pool.GetFreeBlockCount(), 2u);

    pool.Deallocate(a);
    pool.Deallocate(b);
    XR3D_CHECK_EQ(pool.GetFreeBlockCount(), 4u);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_FreeListAllocator_AllocateAndFree)
{
    MemoryManager::Get().Initialize();

    auto& freeList = static_cast<FreeListAllocator&>(MemoryManager::Get().GetAllocator(AllocatorType::FreeList));

    void* a = freeList.Allocate(256);
    XR3D_CHECK_NOT_NULL(a);

    freeList.Deallocate(a);

    void* b = freeList.Allocate(256);
    XR3D_CHECK_NOT_NULL(b);
    XR3D_CHECK_EQ(a, b); // whole freed block should be reused

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_Deallocate_RoutesToCorrectAllocator)
{
    MemoryManager::Get().Initialize();

    // Allocate via Linear (not Default) and free through the generic
    // Deallocate() path - this must not crash or call the wrong
    // allocator's free logic.
    void* p = MemoryManager::Get().Allocate(64, AllocatorType::Linear, MemoryTag::Core, __FILE__, __LINE__);
    XR3D_CHECK_NOT_NULL(p);

    MemoryManager::Get().Deallocate(p);

    MemoryStats stats = MemoryManager::Get().GetGlobalStats();
    XR3D_CHECK_EQ(stats.currentUsage, 0u);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_TEST(Memory_NoLeaksAfterShutdown)
{
    MemoryManager::Get().Initialize();

    void* p = XR3D_ALLOC(16, MemoryTag::Core);
    XR3D_FREE(p);

    XR3D_CHECK(!MemoryManager::Get().GetGlobalStats().currentUsage);

    MemoryManager::Get().Shutdown();
    return true;
}