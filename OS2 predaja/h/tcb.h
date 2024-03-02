//
// Created by os on 6/26/22.
//

#ifndef PROJECT_BASE_TCB_H
#define PROJECT_BASE_TCB_H

#include "../lib/hw.h"
#include "myScheduler.h"
#include "../test/printing.hpp"
#include "MemoryAllocator.h"
#include "slab.h"
enum threadNames
{
    mainThread = 1,
    consoleThread = 2,
    idleThread = 3,
    userMainThread = 4
};
class TCB {
public:
    ~TCB();

    bool isFinished() const {return finished; }
    bool isBlocked() const {return blocked;}
    bool isSleeping() const {return sleeping;}
    bool isSys() const {return sys;}

    void setBlocked(bool value) {TCB::blocked = value; }
    void setFinished(bool value) {TCB::finished = value; }
    void setSleeping(bool value) {TCB::sleeping = value; }
    static void exitThread();


    inline uint64 getTimeSlice() const {
        return timeSlice;
    }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, uint64* stack_space, void* arg, bool needToStart);

    static void yield();

    static TCB *running;

    void switchToSystemStack();
    void switchToUserStack();
    static void putToSleep(time_t t);

    void* operator new(size_t size);
    void* operator new[](size_t size);
    void operator delete(void* ptr);
    void operator delete[](void* ptr);

private:
    explicit TCB (Body body, uint64 timeSlice, uint64* stack_space, void* arg, bool needToStart):
            arg(arg),
            body(body),
            system_stack(body != nullptr ? stack_space : nullptr),
            user_stack(body != nullptr ? system_stack + DEFAULT_STACK_SIZE : nullptr),
            context({
                        (uint64) &threadWrapper,
                        user_stack != nullptr ? (uint64)&(((char*)user_stack)[DEFAULT_STACK_SIZE]) : 0
                        }),
            timeSlice(timeSlice),
            finished(false),
            sys(true),
            nextTCB(nullptr),
            nextSem(nullptr),
            nextSleep(nullptr),
            sleepLeft(0)


    {
        if (body != nullptr) {
            if (needToStart)Scheduler::put(this);
            user_sp = context.sp;
            system_sp = system_stack != nullptr ? (uint64)&(((char*)system_stack)[DEFAULT_STACK_SIZE]) : 0;
        }
        id = cnt++;
        blocked = false;
        sleeping = false;
        if (id != mainThread && id != idleThread && id != consoleThread) sys = false;
        stackSpace = stack_space;

    }

    struct Context{
        uint64 ra;
        uint64 sp;
    };
    void* arg;
    Body body;
    uint64 system_sp;
    uint64 user_sp;
    uint64 *system_stack;
    uint64 *user_stack;
    uint64 *stackSpace;
    Context context;
    uint64 timeSlice;
    bool finished;
    bool blocked;
    bool sleeping;
    bool sys;
    TCB *nextTCB;
    TCB *nextSem;
    TCB *nextSleep;
    static TCB *sleepHead;
    static kmem_cache_t* tcb_cache;
    time_t sleepLeft;

    static int cnt;

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    friend class Scheduler;
    friend class Riscv;
    friend class mySemaphore;

    static void threadWrapper();
    static TCB* findPlaceToSleep(time_t t);

    static uint64 timeSliceCounter;
public:
    int id;


    static void insertAfter(TCB *ptr);
};


#endif //PROJECT_BASE_TCB_H
