//
// Created by os on 8/22/22.
//

#include "../h/myBuffer.h"
extern size_t descSize;

myBuffer::myBuffer(int _cap) : cap(_cap), head(0), tail(0) {
    buffer = (char *)mem_alloc(sizeof(char) * cap);
    itemAvailable = mySemaphore::createSem(0);
    spaceAvailable = mySemaphore::createSem(_cap);
    mutexHead = mySemaphore::createSem(1);
    mutexTail = mySemaphore::createSem(1);

}

myBuffer::~myBuffer() {
    putc('\n');
    printString("Buffer deleted!\n");
    while (getCnt() > 0) {
        char ch = buffer[head];
        putc(ch);
        head = (head + 1) % cap;
    }
    putc('!');
    putc('\n');

    MemoryAllocator::mem_free(buffer);
    delete itemAvailable;
    delete spaceAvailable;
    delete mutexTail;
    delete mutexHead;
}

void myBuffer::put(char val) {
    spaceAvailable->wait();
    mutexTail->wait();

    buffer[tail] = val;
    tail = (tail + 1) % cap;

    mutexTail->signal();
    itemAvailable->signal();

}

int myBuffer::get() {
    itemAvailable->wait();
    mutexHead->wait();

    char ret = buffer[head];
    head = (head + 1) % cap;

    mutexHead->signal();
    spaceAvailable->signal();

    return ret;
}

int myBuffer::getCnt() {
    int ret;

    mutexHead->wait();
    mutexTail->wait();

    if (tail >= head) {
        ret = tail - head;
    } else {
        ret = cap - head + tail;
    }

    mutexTail->signal();
    mutexHead->signal();

    return ret;
}

void *myBuffer::operator new(size_t size) {
    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
           (size + descSize) / MEM_BLOCK_SIZE + 1);

    return MemoryAllocator::mem_alloc(size);
}


void myBuffer::operator delete(void *ptr) {
    MemoryAllocator::mem_free(ptr);
}

void *myBuffer::operator new[](size_t size) {
    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);

    return MemoryAllocator::mem_alloc(size);
}

void myBuffer::operator delete[](void *ptr) {
    MemoryAllocator::mem_free(ptr);
}
