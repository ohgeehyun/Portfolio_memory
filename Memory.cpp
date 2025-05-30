#include "pch.h"
#include "memory.h"
#include "MemoryPool.h"

Memory::Memory()
{
    int32 size = 0;
    int32 tableIndex = 0;
    for (size = 32; size <= 1024; size += 32)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);

        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
    for (; size <= 2048; size += 128)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);
        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
    for (; size <= 4096; size += 256)
    {
        MemoryPool* pool = new MemoryPool(size);
        _pools.push_back(pool);
        while (tableIndex <= size)
        {
            _poolTable[tableIndex] = pool;
            tableIndex++;
        }
    }
}

Memory::~Memory()
{
    for (MemoryPool* pool : _pools)
        delete pool;

    _pools.clear();
}

void* Memory::Allocate(int32 size)
{
    MemoryHeader* header = nullptr;
    const int32 allocSize = size + sizeof(MemoryHeader);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        //일반 할당
        header = reinterpret_cast<MemoryHeader*>(_aligned_malloc(allocSize, SLIST_ALIGNMENT));
    }
    else
    {
        header = _poolTable[allocSize]->Pop();
    }

    return MemoryHeader::AttachHeader(header, allocSize);
}

void Memory::Release(void* ptr)
{
    MemoryHeader* header = MemoryHeader::DetacchHeader(ptr);
    const int32 allocSize = header->allocSize;
    ASSERT_CRASH(allocSize > 0);

    if (allocSize > MAX_ALLOC_SIZE)
    {
        //메모리 풀링 최대 크기를 벗어나면 일반해제
        _aligned_free(header);
    }
    else
    {
        //메모리 풀에 반납
        _poolTable[allocSize]->Push(header);
    }
}