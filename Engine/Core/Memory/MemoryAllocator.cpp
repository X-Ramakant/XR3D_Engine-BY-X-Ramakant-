/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements the Default, Linear, Stack and FreeList allocator strategies.

*/

#include "MemoryAllocator.h"

#include <cstdlib>
#include <cassert>

namespace XR3D::Core::Memory
{
    namespace
    {
        size_t AlignUp(size_t value, size_t alignment)
        {
            return (value + (alignment - 1)) & ~(alignment - 1);
        }
    }

    // ---------------- DefaultAllocator ----------------

    void* DefaultAllocator::Allocate(size_t size, size_t alignment)
    {
        (void)alignment;
        return std::malloc(size);
    }

    void DefaultAllocator::Deallocate(void* pointer)
    {
        std::free(pointer);
    }

    void DefaultAllocator::Reset()
    {
        // Nothing to do - the OS heap owns individual allocations.
    }

    // ---------------- LinearAllocator ----------------

    LinearAllocator::LinearAllocator(size_t capacity)
        : m_capacity(capacity)
    {
        m_buffer = static_cast<uint8_t*>(std::malloc(capacity));
        assert(m_buffer != nullptr && "LinearAllocator failed to allocate backing buffer");
        // TODO (Future): report allocation failure through Core/Diagnostics
        // once that system exists, instead of only asserting here.
    }

    LinearAllocator::~LinearAllocator()
    {
        std::free(m_buffer);
    }

    void* LinearAllocator::Allocate(size_t size, size_t alignment)
    {
        size_t alignedOffset = AlignUp(m_offset, alignment);
        if (alignedOffset + size > m_capacity)
        {
            return nullptr; // TODO (Future): report out-of-memory via Diagnostics.
        }

        void* result = m_buffer + alignedOffset;
        m_offset = alignedOffset + size;
        return result;
    }

    void LinearAllocator::Deallocate(void* pointer)
    {
        (void)pointer;
        // Intentional no-op - see class comment.
    }

    void LinearAllocator::Reset()
    {
        m_offset = 0;
    }

    // ---------------- StackAllocator ----------------

    StackAllocator::StackAllocator(size_t capacity)
        : m_capacity(capacity)
    {
        m_buffer = static_cast<uint8_t*>(std::malloc(capacity));
        assert(m_buffer != nullptr && "StackAllocator failed to allocate backing buffer");
        // TODO (Future): report allocation failure through Core/Diagnostics
        // once that system exists, instead of only asserting here.
    }

    StackAllocator::~StackAllocator()
    {
        std::free(m_buffer);
    }

    void* StackAllocator::Allocate(size_t size, size_t alignment)
    {
        size_t alignedOffset = AlignUp(m_offset, alignment);
        if (alignedOffset + size > m_capacity)
        {
            return nullptr;
        }

        void* result = m_buffer + alignedOffset;
        m_offset = alignedOffset + size;
        return result;
    }

    void StackAllocator::Deallocate(void* pointer)
    {
        // Individual deallocate is not supported - callers must use
        // GetMarker()/RewindToMarker() to free in LIFO order.
        (void)pointer;
        assert(false && "StackAllocator requires RewindToMarker for deallocation");
    }

    void StackAllocator::Reset()
    {
        m_offset = 0;
    }

    StackAllocator::Marker StackAllocator::GetMarker() const
    {
        return m_offset;
    }

    void StackAllocator::RewindToMarker(Marker marker)
    {
        assert(marker <= m_offset && "Cannot rewind forward");
        m_offset = marker;
    }

    // ---------------- FreeListAllocator ----------------

    FreeListAllocator::FreeListAllocator(size_t capacity)
        : m_capacity(capacity)
    {
        m_buffer = static_cast<uint8_t*>(std::malloc(capacity));
        assert(m_buffer != nullptr && "FreeListAllocator failed to allocate backing buffer");

        if (m_buffer != nullptr)
        {
            m_freeList = reinterpret_cast<FreeBlock*>(m_buffer);
            m_freeList->size = capacity;
            m_freeList->next = nullptr;
        }
        else
        {
            m_freeList = nullptr;
            // TODO (Future): report allocation failure through Core/Diagnostics
            // once that system exists, instead of only asserting here.
        }
    }

    FreeListAllocator::~FreeListAllocator()
    {
        std::free(m_buffer);
    }

    void* FreeListAllocator::Allocate(size_t size, size_t alignment)
    {
        (void)alignment;

        // TODO (Future): honor alignment and add best-fit search.
        FreeBlock* previous = nullptr;
        FreeBlock* current = m_freeList;

        while (current != nullptr)
        {
            if (current->size >= size)
            {
                if (previous != nullptr)
                {
                    previous->next = current->next;
                }
                else
                {
                    m_freeList = current->next;
                }
                return reinterpret_cast<void*>(current);
            }

            previous = current;
            current = current->next;
        }

        return nullptr; // Out of memory in this allocator's arena.
    }

    void FreeListAllocator::Deallocate(void* pointer)
    {
        // TODO (Future): merge with adjacent free blocks to reduce
        // fragmentation once real allocation patterns are profiled.
        auto* block = reinterpret_cast<FreeBlock*>(pointer);
        block->next = m_freeList;
        m_freeList = block;
    }

    void FreeListAllocator::Reset()
    {
        if (m_buffer == nullptr)
        {
            m_freeList = nullptr;
            return;
        }

        m_freeList = reinterpret_cast<FreeBlock*>(m_buffer);
        m_freeList->size = m_capacity;
        m_freeList->next = nullptr;
    }
}