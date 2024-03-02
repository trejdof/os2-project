//
// Created by os on 8/16/22.
//
#include "../h/syscall_c.h"

#ifndef PROJECT_BASE_SEMAPHORE_H
#define PROJECT_BASE_SEMAPHORE_H


class mySemaphore {
public:
    mySemaphore (unsigned init) : dealoc(false), val(init), head(nullptr), tail(nullptr){}

    int wait ();
    void signal ();

    int getValue () const { return val; }

    bool isDealocated() const {return dealoc;}
    void setDealoc(bool val) { dealoc = val;}

    void* operator new(size_t size);
    void* operator new[](size_t size);
    void operator delete(void* ptr);
    void operator delete[](void* ptr);

    static mySemaphore *createSem(uint64 init);
private:
    bool dealoc;
    int val;
    TCB* head;
    TCB* tail;

    void block();
    void unblock();

};


#endif //PROJECT_BASE_SEMAPHORE_H
