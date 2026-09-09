/*

Development By : XR Corporation

Software Name : XR 3D Engine

Founder Of XR Corporation : Ramakant

File Responsibility:

* Declares MemoryPool, a fixed block-size allocator built for many
* same-sized allocations (e.g. ECS components, small objects).

*/

#pragma once

#include "MemoryAllocator.h"

namespace XR3D::Core::Memory
{
    class MemoryPool final : public IAllocator
    {
    public:
        MemoryPool(size_t blockSize, size_t blockCount);
        ~MemoryPool() override;

        void* Allocate(size_t size, size_t alignment = kDefaultAlignment) override;
        void  Deallocate(void* pointer) override;
        void  Reset() override;
        const char* GetName() const override { return "MemoryPool"; }
        AllocatorType GetType() const override { return AllocatorType::Pool; }

        size_t GetBlockSize()  const { return m_blockSize; }
        size_t GetBlockCount() const { return m_blockCount; }
        size_t GetFreeBlockCount() const;

    private:
        struct FreeNode { FreeNode* next; };

        uint8_t*  m_buffer     = nullptr;
        size_t    m_blockSize  = 0;
        size_t    m_blockCount = 0;
        FreeNode* m_freeList   = nullptr;

        // TODO (Future): support growable pools that chain additional
        // blocks together once a fixed pool is exhausted.
    };
}