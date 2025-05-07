#include "memory_manager.h"

MemoryManager::MemoryManager(size_t pool_size, size_t chunk_size)
    : _chunkSize(chunk_size),
      _poolStorage(pool_size * chunk_size)
{
    // carve up the pool into chunks
    _freeList.reserve(pool_size);
    uint8_t* base = _poolStorage.data();
    for (size_t i = 0; i < pool_size; ++i) {
        _freeList.push_back(base + i * chunk_size);
    }
}

void* MemoryManager::allocate()
{
    std::lock_guard<std::mutex> g(_lock);
    if (_freeList.empty()) {
        return nullptr;  // pool exhausted
    }
    void* ptr = _freeList.back();
    _freeList.pop_back();
    return ptr;
}

void MemoryManager::deallocate(void* ptr)
{
    std::lock_guard<std::mutex> g(_lock);
    _freeList.push_back(ptr);
}
