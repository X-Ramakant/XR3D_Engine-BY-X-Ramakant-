/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements MemoryManager.

*/

#include "MemoryManager.h"

#include <cassert>

namespace XR3D::Core::Memory
{
    MemoryManager& MemoryManager::Get()
    {
        static MemoryManager instance;
        return instance;
    }

    void MemoryManager::Initialize()
    {
        if (m_initialized)
        {
            return;
        }

        m_defaultAllocator  = std::make_unique<DefaultAllocator>();
        m_linearAllocator   = std::make_unique<LinearAllocator>(kLinearArenaSize);
        m_stackAllocator    = std::make_unique<StackAllocator>(kStackArenaSize);
        m_freeListAllocator = std::make_unique<FreeListAllocator>(kFreeListArenaSize);

        m_initialized = true;
    }

    void MemoryManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        if (m_tracker.HasLeaks())
        {
            m_tracker.DumpReport();
            // TODO (Future): route this through Core/Diagnostics once
            // that system exists, instead of only printing to stdout.
        }

        m_pools.clear();
        m_freeListAllocator.reset();
        m_stackAllocator.reset();
        m_linearAllocator.reset();
        m_defaultAllocator.reset();

        m_initialized = false;
    }

    IAllocator& MemoryManager::ResolveAllocator(AllocatorType type)
    {
        switch (type)
        {
            case AllocatorType::Linear:   return *m_linearAllocator;
            case AllocatorType::Stack:    return *m_stackAllocator;
            case AllocatorType::FreeList: return *m_freeListAllocator;
            case AllocatorType::Default:
            default:                     return *m_defaultAllocator;
        }
    }

    void* MemoryManager::Allocate(size_t size, AllocatorType allocatorType, MemoryTag tag,
                                   const char* file, int line, size_t alignment)
    {
        assert(m_initialized && "MemoryManager::Initialize must be called first");

        IAllocator& allocator = ResolveAllocator(allocatorType);
        void* pointer = allocator.Allocate(size, alignment);

        if (pointer != nullptr)
        {
            m_tracker.OnAllocate(pointer, size, allocatorType, tag, file, line);
        }

        return pointer;
    }

    void MemoryManager::Deallocate(void* pointer)
    {
        if (pointer == nullptr)
        {
            return;
        }

        AllocationRecord record;
        if (!m_tracker.TryGetRecord(pointer, record))
        {
            return; // Untracked/unknown pointer - ignore.
        }

        IAllocator& allocator = ResolveAllocator(record.allocator);
        m_tracker.OnDeallocate(pointer);
        allocator.Deallocate(pointer);
    }

    IAllocator& MemoryManager::GetAllocator(AllocatorType type)
    {
        assert(m_initialized && "MemoryManager::Initialize must be called first");
        return ResolveAllocator(type);
    }

    MemoryPool& MemoryManager::CreatePool(size_t blockSize, size_t blockCount)
    {
        m_pools.push_back(std::make_unique<MemoryPool>(blockSize, blockCount));
        return *m_pools.back();
    }

    MemoryStats MemoryManager::GetGlobalStats() const
    {
        return m_tracker.GetGlobalStats();
    }

    MemoryStats MemoryManager::GetStatsForTag(MemoryTag tag) const
    {
        return m_tracker.GetStatsForTag(tag);
    }

    void MemoryManager::DumpReport() const
    {
        m_tracker.DumpReport();
    }
}