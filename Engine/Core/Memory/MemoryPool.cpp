/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Implements MemoryPool.

*/

#include "MemoryPool.h"

#include <cstdlib>
#include <cassert>

namespace XR3D::Core::Memory
{
    MemoryPool::MemoryPool(size_t blockSize, size_t blockCount)
        : m_blockSize(blockSize < sizeof(FreeNode) ? sizeof(FreeNode) : blockSize)
        , m_blockCount(blockCount)
    {
        m_buffer = static_cast<uint8_t*>(std::malloc(m_blockSize * m_blockCount));
        assert(m_buffer != nullptr && "MemoryPool failed to allocate backing buffer");
        // TODO (Future): report allocation failure through Core/Diagnostics
        // once that system exists, instead of only asserting here.
        Reset();
    }

    MemoryPool::~MemoryPool()
    {
        std::free(m_buffer);
    }

    void* MemoryPool::Allocate(size_t size, size_t alignment)
    {
        (void)alignment;
        assert(size <= m_blockSize && "Requested size exceeds pool block size");

        if (m_freeList == nullptr)
        {
            return nullptr; // Pool exhausted.
        }

        FreeNode* node = m_freeList;
        m_freeList = node->next;
        return node;
    }

    void MemoryPool::Deallocate(void* pointer)
    {
        auto* node = reinterpret_cast<FreeNode*>(pointer);
        node->next = m_freeList;
        m_freeList = node;
    }

    void MemoryPool::Reset()
    {
        m_freeList = nullptr;

        if (m_buffer == nullptr)
        {
            return;
        }

        for (size_t i = 0; i < m_blockCount; ++i)
        {
            auto* node = reinterpret_cast<FreeNode*>(m_buffer + i * m_blockSize);
            node->next = m_freeList;
            m_freeList = node;
        }
    }

    size_t MemoryPool::GetFreeBlockCount() const
    {
        size_t count = 0;
        for (FreeNode* node = m_freeList; node != nullptr; node = node->next)
        {
            ++count;
        }
        return count;
    }
}