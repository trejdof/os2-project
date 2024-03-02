//
// Created by os on 6/24/22.
//

#ifndef PROJECT_BASE_SYSCALL_C_H
#define PROJECT_BASE_SYSCALL_C_H

#include "../lib/mem.h"
#include "../h/MemoryAllocator.h"
#include "tcb.h"
#include "mySemaphore.h"

typedef mySemaphore* sem_t;
typedef TCB* thread_t;
typedef unsigned long time_t;

// Memory related syscalls
void* mem_alloc (size_t size);
int mem_free (void*);
const int EOF = -1;

// Thread related syscalls
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg);
int thread_exit ();
void thread_dispatch();
void thread_start(thread_t);
int thread_create_no_start(thread_t* handle, void(*start_routine)(void*), void* arg);
int time_sleep (time_t);

// Semaphore related syscalls
int sem_open (sem_t* handle, unsigned init);
int sem_close (sem_t handle);
int sem_wait (sem_t id);
int sem_signal (sem_t id);

// Console related syscalls
char getc ();
void putc (char);



void syscallWrapper(...);
#endif //PROJECT_BASE_SYSCALL_C_H
