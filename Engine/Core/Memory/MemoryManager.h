/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares MemoryManager, the single entry point for all engine memory
* allocation. Owns every allocator strategy and the MemoryTracker.

*/

#pragma once

#include "MemoryAllocator.h"
#include "MemoryPool.h"
#include "MemoryTracker.h"
#include "MemoryTypes.h"

#include <memory>
#include <vector>

// Convenience macros so call sites automatically capture file/line for
// leak reporting, without hand-writing them at every call site.

#define XR3D_ALLOC(size, tag) \
    ::XR3D::Core::Memory::MemoryManager::Get().Allocate((size), ::XR3D::Core::Memory::AllocatorType::Default, (tag), __FILE__, __LINE__)

#define XR3D_FREE(pointer) \
    ::XR3D::Core::Memory::MemoryManager::Get().Deallocate((pointer))

namespace XR3D::Core::Memory
{
    class MemoryManager
    {
    public:
        static MemoryManager& Get();

        void Initialize();
        void Shutdown();

        void* Allocate(size_t size,
                        AllocatorType allocatorType = AllocatorType::Default,
                        MemoryTag tag = MemoryTag::Untagged,
                        const char* file = nullptr,
                        int line = 0,
                        size_t alignment = kDefaultAlignment);

        void Deallocate(void* pointer);

        // Direct access to a specific strategy, for callers that need
        // more control (e.g. a per-frame LinearAllocator scratch arena).
        IAllocator& GetAllocator(AllocatorType type);
        MemoryPool& CreatePool(size_t blockSize, size_t blockCount);

        MemoryStats GetGlobalStats() const;
        MemoryStats GetStatsForTag(MemoryTag tag) const;
        void DumpReport() const;

    private:
        MemoryManager() = default;
        ~MemoryManager() = default;
        MemoryManager(const MemoryManager&) = delete;
        MemoryManager& operator=(const MemoryManager&) = delete;

        bool m_initialized = false;

        std::unique_ptr<DefaultAllocator>   m_defaultAllocator;
        std::unique_ptr<LinearAllocator>    m_linearAllocator;
        std::unique_ptr<StackAllocator>     m_stackAllocator;
        std::unique_ptr<FreeListAllocator>  m_freeListAllocator;
        std::vector<std::unique_ptr<MemoryPool>> m_pools;

        MemoryTracker m_tracker;

        // Fixed scratch sizes for the built-in Linear/Stack/FreeList
        // arenas. TODO (Future): make these configurable via Core/Config
        // once that system is implemented.
        static constexpr size_t kLinearArenaSize   = 8  * 1024 * 1024; // 8 MB
        static constexpr size_t kStackArenaSize    = 4  * 1024 * 1024; // 4 MB
        static constexpr size_t kFreeListArenaSize = 16 * 1024 * 1024; // 16 MB

        IAllocator& ResolveAllocator(AllocatorType type);
    };
}