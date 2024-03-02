//
// Created by os on 8/16/22.
//

#include "../h/mySemaphore.h"

extern size_t descSize;

mySemaphore *mySemaphore::createSem(uint64 init) {
    mySemaphore* test = new mySemaphore(init);
    return test;
}

int mySemaphore::wait() {
    if (--val < 0) block();
    if (isDealocated()) return -1;
    return 0;
}

void mySemaphore::signal() {
    if (++val<=0) unblock();
}

void mySemaphore::block() {
    if (head != nullptr){
        tail->nextSem = TCB::running;
        tail = TCB::running;
    }
    else{
        head = TCB::running;
        tail = TCB::running;
    }
    TCB::running->setBlocked(true);
    TCB::dispatch();

}

void mySemaphore::unblock() {
    if (head == nullptr) tail = nullptr;
    else {
        thread_t tmp = head;
        head = head->nextSem;
        tmp->nextSem = nullptr;
        tmp->setBlocked(false);
        Scheduler::put(tmp);
    }

}
void *mySemaphore::operator new(size_t size) {
    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);

    return MemoryAllocator::mem_alloc(size);
}

void mySemaphore::operator delete(void *ptr) {
    MemoryAllocator::mem_free(ptr);
}

void *mySemaphore::operator new[](size_t size) {

    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);

    return MemoryAllocator::mem_alloc(size);
}

void mySemaphore::operator delete[](void *ptr) {
    MemoryAllocator::mem_free(ptr);
}


