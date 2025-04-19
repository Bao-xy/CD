#include "hash_index.h"

int main()
{
    SharedMemoryPool &pool = SharedMemoryPool::getInstance(true); // create
    void *base = pool.get_base();
    printf("base: %p\n", base);
    void* mem = pool.allocate(sizeof(size_t)+sizeof(HashIndex));
    printf("mem: %p\n", mem);
    // 构造 HashIndex
    HashIndex *index = new (mem + sizeof(size_t)) HashIndex();
    printf("index: %p\n", index);

    index->construct(MyAllocator<Entry>(true));

    // 写 OffsetPtr
    OffsetPtr<HashIndex> index_ptr(index, base);
    *reinterpret_cast<size_t *>(base) = index_ptr.offset; // 存储在共享内存起始处

    index->add("apple", "red");
    index->add("banana", "yellow");
    index->print();
}
