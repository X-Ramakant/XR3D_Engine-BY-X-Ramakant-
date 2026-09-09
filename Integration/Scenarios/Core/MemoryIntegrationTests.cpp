/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Integration scenarios for Core/Memory: verifies realistic engine-like
* usage patterns (per-frame scratch memory, multi-system tagged
* allocations, nested stack scopes, pool exhaustion) stay crash-free
* and leak-free once Memory is running inside the Engine.

*/

#include "IntegrationTestFramework.h"
#include "Core\Memory\MemoryManager.h"

#include <vector>

using namespace XR3D::Core::Memory;

XR3D_SCENARIO(Integration_Memory_SimulatedFrameLoop_NoLeaks)
{
    MemoryManager::Get().Initialize();

    auto& linear = static_cast<LinearAllocator&>(MemoryManager::Get().GetAllocator(AllocatorType::Linear));

    const int kFrameCount = 60; // simulate roughly one second at 60 FPS
    for (int frame = 0; frame < kFrameCount; ++frame)
    {
        void* renderScratch = linear.Allocate(256);
        void* physicsScratch = linear.Allocate(128);
        XR3D_CHECK_NOT_NULL(renderScratch);
        XR3D_CHECK_NOT_NULL(physicsScratch);

        // End of frame: reset the linear arena for reuse next frame.
        linear.Reset();
    }

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Memory_MultiSystem_TaggedAllocations_NoLeaks)
{
    MemoryManager::Get().Initialize();

    std::vector<void*> renderAllocs;
    std::vector<void*> physicsAllocs;
    std::vector<void*> audioAllocs;

    for (int i = 0; i < 20; ++i)
    {
        renderAllocs.push_back(MemoryManager::Get().Allocate(64, AllocatorType::Default, MemoryTag::Rendering, __FILE__, __LINE__));
        physicsAllocs.push_back(MemoryManager::Get().Allocate(48, AllocatorType::Default, MemoryTag::Physics, __FILE__, __LINE__));
        audioAllocs.push_back(MemoryManager::Get().Allocate(32, AllocatorType::Default, MemoryTag::Audio, __FILE__, __LINE__));
    }

    for (void* p : renderAllocs) { XR3D_CHECK_NOT_NULL(p); MemoryManager::Get().Deallocate(p); }
    for (void* p : physicsAllocs) { XR3D_CHECK_NOT_NULL(p); MemoryManager::Get().Deallocate(p); }
    for (void* p : audioAllocs) { XR3D_CHECK_NOT_NULL(p); MemoryManager::Get().Deallocate(p); }

    MemoryStats stats = MemoryManager::Get().GetGlobalStats();
    XR3D_CHECK_EQ(stats.currentUsage, 0u);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Memory_StackAllocator_NestedScopes)
{
    MemoryManager::Get().Initialize();

    auto& stack = static_cast<StackAllocator&>(MemoryManager::Get().GetAllocator(AllocatorType::Stack));

    auto outerMarker = stack.GetMarker();
    void* outer = stack.Allocate(64);
    XR3D_CHECK_NOT_NULL(outer);

    {
        auto innerMarker = stack.GetMarker();
        void* inner = stack.Allocate(32);
        XR3D_CHECK_NOT_NULL(inner);
        stack.RewindToMarker(innerMarker);
    }

    stack.RewindToMarker(outerMarker);

    MemoryManager::Get().Shutdown();
    return true;
}

XR3D_SCENARIO(Integration_Memory_Pool_ExhaustionHandledGracefully)
{
    MemoryManager::Get().Initialize();

    MemoryPool& pool = MemoryManager::Get().CreatePool(sizeof(int), 2);

    void* a = pool.Allocate(sizeof(int));
    void* b = pool.Allocate(sizeof(int));
    void* c = pool.Allocate(sizeof(int)); // pool is now exhausted

    XR3D_CHECK_NOT_NULL(a);
    XR3D_CHECK_NOT_NULL(b);
    XR3D_CHECK_NULL(c); // must fail gracefully by returning nullptr, not crash

    pool.Deallocate(a);
    pool.Deallocate(b);

    MemoryManager::Get().Shutdown();
    return true;
}