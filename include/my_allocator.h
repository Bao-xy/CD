#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#define SHM_NAME_2 "/my_shared_memory"
#define SHM_SIZE_2 65536

// ------------------------
// OffsetPtr
// ------------------------
template <typename T>
struct OffsetPtr {
    size_t offset = 0;

    OffsetPtr() = default;

    OffsetPtr(T* ptr, void* base) {
        offset = reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(base);
    }

    T* get(void* base) const {
        return reinterpret_cast<T*>(reinterpret_cast<char*>(base) + offset);
    }

    // T& operator* (void* base) const { return *get(base); }
    // T* operator->(void* base) const { return get(base); }
};

// ------------------------
// 共享内存池
// ------------------------
class SharedMemoryPool {
public:
    SharedMemoryPool(bool create = false) {
        if (create) {
            shm_fd = shm_open(SHM_NAME_2, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, SHM_SIZE_2);
        } else {
            shm_fd = shm_open(SHM_NAME_2, O_RDWR, 0666);
            if (shm_fd == -1) throw std::runtime_error("Failed to open shared memory");
        }

        base = (char*)mmap(NULL, SHM_SIZE_2, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (base == MAP_FAILED) throw std::runtime_error("Failed to mmap shared memory");

        bump_offset = reinterpret_cast<size_t*>(base);
        memory_start = base + sizeof(size_t);

        if (create) std::memset(base, 0, SHM_SIZE_2);
    }

    ~SharedMemoryPool() {
        munmap(base, SHM_SIZE_2);
        close(shm_fd);
    }

    void* allocate(size_t size) {
        size_t offset = *bump_offset;
        if (offset + size > SHM_SIZE_2 - sizeof(size_t)) throw std::bad_alloc();
        void* ptr = memory_start + offset;
        *bump_offset += size;
        return ptr;
    }

    void* get_base() const { return memory_start; }

    static SharedMemoryPool& getInstance(bool create = false) {
        static SharedMemoryPool instance(create);
        return instance;
    }

private:
    int shm_fd;
    char* base;
    size_t* bump_offset;
    char* memory_start;
};

// ------------------------
// STL allocator
// ------------------------
template <typename T>
class MyAllocator {
public:
    using value_type = T;

    MyAllocator(bool create = false) {
        pool = &SharedMemoryPool::getInstance(create);
    }

    template <typename U>
    MyAllocator(const MyAllocator<U>& other): pool(other.pool) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(pool->allocate(n * sizeof(T)));
    }

    void deallocate(T*, std::size_t) {}

    template <typename U>
    struct rebind {
        using other = MyAllocator<U>;
    };

    SharedMemoryPool* pool;
    using is_always_equal = std::false_type;
};

#endif
