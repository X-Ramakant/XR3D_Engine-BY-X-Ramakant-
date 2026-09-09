/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements MemoryTracker.

*/

#include "MemoryTracker.h"

#include <cstdio>

namespace XR3D::Core::Memory
{
    void MemoryTracker::OnAllocate(void* pointer, size_t size, AllocatorType allocator,
                                    MemoryTag tag, const char* file, int line)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        AllocationRecord record;
        record.pointer    = pointer;
        record.size       = size;
        record.allocator  = allocator;
        record.tag        = tag;
        record.sourceFile = file;
        record.sourceLine = line;

        m_activeAllocations[pointer] = record;

        m_globalStats.currentUsage += size;
        m_globalStats.totalAllocated += size;
        m_globalStats.allocationCount += 1;
        if (m_globalStats.currentUsage > m_globalStats.peakUsage)
        {
            m_globalStats.peakUsage = m_globalStats.currentUsage;
        }

        MemoryStats& tagStats = m_tagStats[static_cast<size_t>(tag)];
        tagStats.currentUsage += size;
        tagStats.totalAllocated += size;
        tagStats.allocationCount += 1;
        if (tagStats.currentUsage > tagStats.peakUsage)
        {
            tagStats.peakUsage = tagStats.currentUsage;
        }
    }

    void MemoryTracker::OnDeallocate(void* pointer)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_activeAllocations.find(pointer);
        if (it == m_activeAllocations.end())
        {
            return; // Untracked pointer - ignore.
        }

        const AllocationRecord& record = it->second;

        m_globalStats.currentUsage -= record.size;
        m_globalStats.totalFreed += record.size;
        m_globalStats.deallocationCount += 1;

        MemoryStats& tagStats = m_tagStats[static_cast<size_t>(record.tag)];
        tagStats.currentUsage -= record.size;
        tagStats.totalFreed += record.size;
        tagStats.deallocationCount += 1;

        m_activeAllocations.erase(it);
    }

    bool MemoryTracker::TryGetRecord(void* pointer, AllocationRecord& outRecord) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_activeAllocations.find(pointer);
        if (it == m_activeAllocations.end())
        {
            return false;
        }

        outRecord = it->second;
        return true;
    }

    MemoryStats MemoryTracker::GetGlobalStats() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_globalStats;
    }

    MemoryStats MemoryTracker::GetStatsForTag(MemoryTag tag) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tagStats[static_cast<size_t>(tag)];
    }

    bool MemoryTracker::HasLeaks() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_activeAllocations.empty();
    }

    void MemoryTracker::DumpReport() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::printf("---- Memory Report ----\n");
        std::printf("Current usage : %zu bytes\n", m_globalStats.currentUsage);
        std::printf("Peak usage    : %zu bytes\n", m_globalStats.peakUsage);
        std::printf("Allocations   : %zu\n", m_globalStats.allocationCount);
        std::printf("Deallocations : %zu\n", m_globalStats.deallocationCount);

        if (!m_activeAllocations.empty())
        {
            std::printf("LEAKS DETECTED: %zu block(s) still allocated\n", m_activeAllocations.size());
            for (const auto& pair : m_activeAllocations)
            {
                const AllocationRecord& record = pair.second;
                std::printf("  - %zu bytes at %s:%d\n", record.size,
                            record.sourceFile ? record.sourceFile : "unknown",
                            record.sourceLine);
            }
        }
    }
}