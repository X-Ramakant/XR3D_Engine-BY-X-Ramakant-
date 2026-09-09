/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares MemoryTracker, which records every allocation/deallocation
* for leak detection and per-tag usage reporting.

*/

#pragma once

#include "MemoryTypes.h"

#include <mutex>
#include <unordered_map>

namespace XR3D::Core::Memory
{
    class MemoryTracker
    {
    public:
        MemoryTracker() = default;

        void OnAllocate(void* pointer, size_t size, AllocatorType allocator,
                         MemoryTag tag, const char* file, int line);
        void OnDeallocate(void* pointer);

        // Looks up the record for an active pointer, needed so
        // MemoryManager can route Deallocate to the correct allocator.
        bool TryGetRecord(void* pointer, AllocationRecord& outRecord) const;

        MemoryStats GetGlobalStats() const;
        MemoryStats GetStatsForTag(MemoryTag tag) const;

        bool HasLeaks() const;
        void DumpReport() const;

    private:
        mutable std::mutex m_mutex;
        std::unordered_map<void*, AllocationRecord> m_activeAllocations;
        MemoryStats m_globalStats;
        MemoryStats m_tagStats[static_cast<size_t>(MemoryTag::Count)];

        // TODO (Future): add allocation call-stack capture for deeper
        // leak diagnostics once a stack-walking utility exists.
    };
}