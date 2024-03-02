//
// Created by os on 6/26/22.
//


#include "../h/tcb.h"
#include "../h/riscv.h"
extern size_t descSize;

TCB *TCB::running = nullptr;
TCB *TCB::sleepHead = nullptr;
kmem_cache_t* TCB::tcb_cache = nullptr;

int TCB::cnt = 1;

uint64 TCB::timeSliceCounter = 0;
extern TCB* Main;


TCB *TCB::createThread(TCB::Body body, uint64* stack_space, void* arg, bool needToStart) {
    TCB* test = new TCB(body, DEFAULT_TIME_SLICE, stack_space, arg, needToStart);
    return test;
}

void TCB::dispatch() {

    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked() && !old->isSleeping())
        Scheduler::put(old);
    running = Scheduler::get();

    Riscv::pushRegisters();
    TCB::contextSwitch(&old->context, &running->context);
    Riscv::popRegisters();

}

void TCB::exitThread() {
    running->setFinished(true);
}

void TCB::putToSleep(time_t t) {
    running->nextSleep = nullptr;
    if (t == 0) return;
    if (!TCB::running->sleepHead) {
        sleepHead = TCB::running;
        sleepHead->sleepLeft = t;
        sleepHead->nextSleep = nullptr;
    }
    else{
        TCB* ptr = findPlaceToSleep(t);
        if (ptr == nullptr){
            // Insert as head
            TCB::running->nextSleep = sleepHead;
            sleepHead->sleepLeft -= TCB::running->sleepLeft;
            sleepHead = TCB::running;
        }else{
            insertAfter(ptr);
        }
    }
    TCB::running->setSleeping(true);
    TCB::dispatch();
}
static int k = 0;
void TCB::insertAfter(TCB* ptr){
    TCB::running->nextSleep = ptr->nextSleep;
    ptr->nextSleep = TCB::running;
    if (running->nextSleep){
        running->nextSleep->sleepLeft -= running->sleepLeft;
        k++;
    }

}
TCB *TCB::findPlaceToSleep(time_t t) {
    time_t relative = t;
    TCB* place = nullptr;
    TCB* ptr = sleepHead;
    while (ptr){
        if (ptr->sleepLeft > relative) break;
        if (ptr->sleepLeft == relative) {
            relative = 0;
            place = ptr;
            break;
        }
        place = ptr;
        relative = relative - ptr->sleepLeft;
        ptr = ptr->nextSleep;

    }
    TCB::running->sleepLeft = relative;
    return place;
}
TCB::~TCB() {
    MemoryAllocator::mem_free(system_stack);
}
void *TCB::operator new(size_t size) {

    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);
//    if (!tcb_cache){
//        tcb_cache = kmem_cache_create("tcb", size, TCB(), ~TCB());
//
//    }
    void* test = MemoryAllocator::mem_alloc(size);

    return test;
}

void TCB::operator delete(void *ptr) {
    MemoryAllocator::mem_free(ptr);
}

void *TCB::operator new[](size_t size) {

    size = ((size + descSize) % MEM_BLOCK_SIZE == 0 ? (size + descSize) / MEM_BLOCK_SIZE :
            (size + descSize) / MEM_BLOCK_SIZE + 1);

    return MemoryAllocator::mem_alloc(size);
}

void TCB::operator delete[](void *ptr) {
    MemoryAllocator::mem_free(ptr);
}


void TCB::switchToSystemStack() {


    if (running->system_stack != nullptr){
        __asm__ volatile("mv %0, sp" : "=r" (running->user_sp));
        __asm__ volatile("mv sp, %0" : : "r" (running->system_sp));
        __asm__ volatile("addi sp, sp, -16");


    }
    __asm__ volatile("ld x15, (s0)");
    __asm__ volatile("ld x14, 8(s0)");
}


void TCB::switchToUserStack() {
    if (running->system_stack != nullptr) {
       // __asm__ volatile("mv %0, sp" : "=r" (running->system_sp));
        __asm__ volatile("mv sp, %0" : : "r" (running->user_sp));
    }
}

void TCB::threadWrapper() {
    if (!running->isSys())
        Riscv::popSppSpie();
    running->body(running->arg);
    thread_exit();

}











