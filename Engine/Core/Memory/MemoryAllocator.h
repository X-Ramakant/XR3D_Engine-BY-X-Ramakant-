/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares the IAllocator interface and the Default, Linear, Stack and
* FreeList allocator strategies used by the Memory subsystem.

*/

#pragma once

#include "MemoryTypes.h"

namespace XR3D::Core::Memory
{
    // Common interface every allocation strategy implements.
    // MemoryManager is the only class allowed to call these directly.

    class IAllocator
    {
    public:
        virtual ~IAllocator() = default;

        virtual void* Allocate(size_t size, size_t alignment = kDefaultAlignment) = 0;
        virtual void  Deallocate(void* pointer) = 0;
        virtual void  Reset() = 0;
        virtual const char* GetName() const = 0;
        virtual AllocatorType GetType() const = 0;
    };

    // Falls back to the platform heap. Always available, used when no
    // more specialized allocator fits the request.

    class DefaultAllocator final : public IAllocator
    {
    public:
        void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
        void  Deallocate(void* pointer) override;
        void  Reset() override;
        const char* GetName() const override { return "DefaultAllocator"; }
        AllocatorType GetType() const override { return AllocatorType::Default; }
    };

    // Bump allocator over a fixed-size buffer. Extremely fast allocation,
    // no individual Deallocate - the whole arena is freed via Reset().

    class LinearAllocator final : public IAllocator
    {
    public:
        explicit LinearAllocator(size_t capacity);
        ~LinearAllocator() override;

        void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
        void  Deallocate(void* pointer) override; // no-op by design
        void  Reset() override;
        const char* GetName() const override { return "LinearAllocator"; }
        AllocatorType GetType() const override { return AllocatorType::Linear; }

    private:
        uint8_t* m_buffer   = nullptr;
        size_t   m_capacity = 0;
        size_t   m_offset   = 0;
    };

    // LIFO allocator with rewindable markers, useful for per-frame scratch
    // memory where allocations are freed in strict reverse order.

    class StackAllocator final : public IAllocator
    {
    public:
        using Marker = size_t;

        explicit StackAllocator(size_t capacity);
        ~StackAllocator() override;

        void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
        void  Deallocate(void* pointer) override;
        void  Reset() override;
        const char* GetName() const override { return "StackAllocator"; }
        AllocatorType GetType() const override { return AllocatorType::Stack; }

        Marker GetMarker() const;
        void   RewindToMarker(Marker marker);

    private:
        uint8_t* m_buffer   = nullptr;
        size_t   m_capacity = 0;
        size_t   m_offset   = 0;
    };

    // General purpose allocator that can free blocks in any order by
    // maintaining a free list of previously released blocks.

    class FreeListAllocator final : public IAllocator
    {
    public:
        explicit FreeListAllocator(size_t capacity);
        ~FreeListAllocator() override;

        void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
        void  Deallocate(void* pointer) override;
        void  Reset() override;
        const char* GetName() const override { return "FreeListAllocator"; }
        AllocatorType GetType() const override { return AllocatorType::FreeList; }

    private:
        struct FreeBlock
        {
            size_t     size = 0;
            FreeBlock* next = nullptr;
        };

        uint8_t*   m_buffer    = nullptr;
        size_t     m_capacity  = 0;
        FreeBlock* m_freeList  = nullptr;

        // TODO (Future): add best-fit search and adjacent free-block
        // coalescing once real fragmentation data from profiling exists.
    };
}