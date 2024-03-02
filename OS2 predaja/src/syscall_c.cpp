//
// Created by os on 6/24/22.
//

#include "../h/syscall_c.h"
#include "../h/MemoryAllocator.h"
#include "../h/riscv.h"

size_t descSize = sizeof(MemoryAllocator::segDesc);

void *mem_alloc(size_t size){

    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);


    syscallWrapper(0x01,size);
    uint64 test = Riscv::r_a0();
    return (void*)test;

}

int mem_free(void *p){
    syscallWrapper(0x02,p);
    return Riscv::r_a0();

}
int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg){
    void* stack_space = mem_alloc(2*(DEFAULT_STACK_SIZE));
    if (stack_space == nullptr) return -1;

    syscallWrapper(0x11, handle, start_routine, arg, stack_space);

    return 0;
}
void thread_start(thread_t handle){
    syscallWrapper(0x14, handle);
}
int thread_create_no_start(thread_t* handle, void(*start_routine)(void*), void* arg){
    void* stack_space = mem_alloc(2*(DEFAULT_STACK_SIZE));
    if (stack_space == nullptr) return -1;
    syscallWrapper(0x15, handle, start_routine, arg, stack_space);
    return 0;

}
int thread_exit() {
    syscallWrapper(0x12);
    return 0;
}
void thread_dispatch () {
    syscallWrapper(0x13);
}
int sem_open(sem_t *handle, unsigned int init) {
    syscallWrapper(0x21, handle, init);
    return 0;
}
int sem_close (sem_t handle){
    syscallWrapper(0x22, handle);
    return 0;
}
int sem_wait (sem_t id){
    syscallWrapper(0x23, id);
    return Riscv::r_a0();
}
int sem_signal (sem_t id){
    syscallWrapper(0x24, id);
    return 0;
}
int time_sleep (time_t time){
    syscallWrapper(0x31, time);
    return 0;
}
char getc() {
    syscallWrapper(0x41);
    return Riscv::r_a0();
}

void putc(char ch) {
    syscallWrapper(0x42, ch);
}
void syscallWrapper(...){
    __asm__ volatile ("ecall");
}





