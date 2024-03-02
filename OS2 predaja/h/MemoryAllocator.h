#ifndef MEMORY_ALLOCATOR
#define MEMORY_ALLOCATOR

#include "../lib/hw.h"
#include "../lib/console.h"
#include "../test/printing.hpp"


class MemoryAllocator{

public:

    static void* mem_alloc (size_t size);

    static int mem_free (void*);

    static void init(uint64 heap_start);

    struct segDesc {
        size_t size;
        segDesc* next;
        segDesc* prev = nullptr;
        size_t padding; // Padding so stack pointer is multiple of 16
    };
    static segDesc* freeSegHead;

private:
    MemoryAllocator(){};

    static void tryToMerge(segDesc *main, segDesc *curr);
};
#endif //MEMORY_ALLOCATOR


