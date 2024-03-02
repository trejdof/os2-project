//
// Created by os on 8/22/22.
//

#ifndef PROJECT_BASE_TBUFFER_H
#define PROJECT_BASE_TBUFFER_H
#include "../h/syscall_c.h"
#include "../test/printing.hpp"
class myBuffer {
private:
    int cap;
    char *buffer;
    int head, tail;

    sem_t spaceAvailable;
    sem_t itemAvailable;
    sem_t mutexHead;
    sem_t mutexTail;


public:
    myBuffer(int _cap);
    ~myBuffer();

    void put(char val);
    int get();

    int getCnt();
    void* operator new(size_t size);
    void* operator new[](size_t size);
    void operator delete(void* ptr);
    void operator delete[](void* ptr);

};

#endif //PROJECT_BASE_TBUFFER_H
