#ifndef _syscall_cpp
#define _syscall_cpp
#include "../lib/mem.h"
#include "../h/MemoryAllocator.h"
#include "syscall_c.h"

void *operator new(size_t n);

void *operator new[](size_t n);

void operator delete(void *p);

void operator delete[](void *p);

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    static void wrapper(void*);
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};
class PeriodicThread;
struct periodic_struct
{
    PeriodicThread* pt;
    time_t period;
    periodic_struct(PeriodicThread* p, time_t t){
        pt = p;
        period = t;
    }
};
class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period) : Thread(&wrapper, new periodic_struct(this, period)){}
    virtual void periodicActivation () {}
public:
    static void wrapper(void* arg);
};

class Console {
public:
    static char getc ();
    static void putc (char);
};
#endif