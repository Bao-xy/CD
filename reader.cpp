#include "hash_index.h"

int main()
{
    SharedMemoryPool &pool = SharedMemoryPool::getInstance(false);
    void *base = pool.get_base();
    printf("base: %p\n", base);
    // 从共享内存读取 Offset
    size_t offset = *reinterpret_cast<size_t *>(base);
    printf("offset: %zu\n", offset);
    OffsetPtr<HashIndex> index_ptr;
    index_ptr.offset = offset;

    HashIndex *index = index_ptr.get(base);
    printf("index: %p\n", index);
    index->print();
}
