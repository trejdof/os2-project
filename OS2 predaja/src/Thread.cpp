//
// Created by os on 8/24/22.
//

#include "../h/syscall_cpp.h"

Thread::Thread(void (*body)(void *), void *arg) {
    thread_create_no_start(&myHandle,body, arg);
}

Thread::~Thread() {
    // tcb has to be in memory in order to check for finished threads
}

int Thread::start() {
    thread_start(myHandle);
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return time_sleep(t);
}

Thread::Thread() {
    thread_create_no_start(&myHandle, &wrapper, this);
}

void Thread::wrapper(void* arg) {
    Thread* thr = (Thread*) arg;
    thr->run();
}

