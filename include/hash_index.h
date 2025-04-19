#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include <string>
#include <vector>
#include <iostream>
#include "my_allocator.h"

struct Entry {
    char key[32];
    char value[32];
};

struct HashIndex {
    using Alloc = MyAllocator<Entry>;
    using Vec = std::vector<Entry, Alloc>;

    OffsetPtr<Vec> entries_vec;

    void construct(Alloc alloc) {
        void* base = alloc.pool->get_base();
        Vec* vec = new (alloc.allocate(1)) Vec(alloc);
        entries_vec = OffsetPtr<Vec>(vec, base);
    }

    void add(const std::string& k, const std::string& v) {
        void* base = SharedMemoryPool::getInstance().get_base();
        Entry e;
        strncpy(e.key, k.c_str(), 31);
        strncpy(e.value, v.c_str(), 31);
        entries_vec.get(base)->push_back(e);
    }

    void print() {
        void* base = SharedMemoryPool::getInstance().get_base();
        for (const auto& e : *entries_vec.get(base)) {
            std::cout << "Key: " << e.key << ", Value: " << e.value << std::endl;
        }
    }
};

#endif
