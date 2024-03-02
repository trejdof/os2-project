//
// Created by os on 6/24/22.
//
#include "../h/syscall_cpp.h"
#include "../h/MemoryAllocator.h"
#include "../h/syscall_c.h"


void *operator new (size_t size) {
    return mem_alloc(size);
}

void *operator new[] (size_t size) {

    return mem_alloc(size);
}

void operator delete (void *p) {
    mem_free(p);
}

void operator delete[] (void *p) {
    mem_free(p);
}



