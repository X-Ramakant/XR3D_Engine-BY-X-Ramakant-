/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Defines shared types, enums and constants used across the Memory system.

*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace XR3D::Core::Memory
{
    // Identifies which allocation strategy produced/owns a block.
    enum class AllocatorType : uint8_t
    {
        Default = 0,   // Falls back to the OS heap (malloc/free based).
        Linear,        // Bump allocator, no per-allocation free, reset all at once.
        Stack,         // LIFO allocator with rewindable markers.
        Pool,          // Fixed block-size pool allocator.
        FreeList,      // General purpose allocator, allows free in any order.
        Count
    };


    // Categorizes allocations by subsystem, for per-tag memory reporting.
    enum class MemoryTag : uint8_t
    {
        Untagged = 0,
        Core,
        Rendering,
        Physics,
        Audio,
        Scripting,
        Networking,
        UI,
        AI,
        Resource,
        Count
    };


    // Default alignment used when the caller does not request one explicitly.
    constexpr size_t kDefaultAlignment = alignof(std::max_align_t);


    // Metadata recorded for every tracked allocation.
    struct AllocationRecord
    {
        void*          pointer     = nullptr;
        size_t         size        = 0;
        size_t         alignment   = kDefaultAlignment;
        AllocatorType  allocator   = AllocatorType::Default;
        MemoryTag      tag         = MemoryTag::Untagged;
        const char*    sourceFile  = nullptr;
        int            sourceLine  = 0;
    };


    // Aggregate memory usage statistics, global or per-tag.
    struct MemoryStats
    {
        size_t currentUsage      = 0;
        size_t peakUsage         = 0;
        size_t totalAllocated    = 0;
        size_t totalFreed        = 0;
        size_t allocationCount   = 0;
        size_t deallocationCount = 0;
    };

    // TODO (Future): add NUMA-node / GPU-visible memory tag support once
    // Rendering (V4) needs device-local allocations.
}