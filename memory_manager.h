#pragma once

#include <cstddef>
#include <vector>
#include <mutex>

/**
 * A simple fixed‐size static‐pool allocator.
 * Eliminates heap fragmentation for control‐loop buffers.
 */
class MemoryManager {
public:
    MemoryManager(size_t pool_size = 512, size_t chunk_size = 256);
    ~MemoryManager() = default;

    void* allocate();
    void  deallocate(void* ptr);

private:
    const size_t _chunkSize;
    std::vector<uint8_t> _poolStorage;
    std::vector<void*>  _freeList;
    std::mutex          _lock;
};
