//
// Created by marko on 20.4.22..
//

#include "../h/riscv.h"
#include "../test/printing.hpp"
#include "../h/mySemaphore.h"
#include "../h/myConsole.h"

void Riscv::popSppSpie()
{
    mc_sstatus(Riscv::SSTATUS_SPP);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}
void mem_allocWrapper(uint64 a1){
    uint64 size = a1;
    MemoryAllocator::mem_alloc(size);

}
void mem_freeWrapper(uint64 a1) {
    void* ptr = (void*)a1;
    MemoryAllocator::mem_free(ptr);
}
void thread_createWrapper(uint64 handle, uint64 start_routine, uint64 arg, uint64 stack_space) {
    TCB** myHandle = (TCB**)handle;
    *myHandle = TCB::createThread(reinterpret_cast<void (*)(void *)>(start_routine),
                                                        reinterpret_cast<uint64 *>(stack_space),
                                                        reinterpret_cast<void *>(arg), true);

}

void thread_exitWrapper(){
    TCB::exitThread();

}
void thread_startWrapper(uint64 handle){
    TCB* myHandle = (TCB*) handle;
    Scheduler::put(myHandle);

}
void thread_create_no_startWrapper(uint64 handle, uint64 start_routine, uint64 arg, uint64 stack_space) {
    TCB** myHandle = (TCB**)handle;
    *myHandle = TCB::createThread(reinterpret_cast<void (*)(void *)>(start_routine),
                                  reinterpret_cast<uint64 *>(stack_space),
                                  reinterpret_cast<void *>(arg),false);

}
void thread_dispatchWrapper(){
    //solved by switch

}
void sem_openWrapper(uint64 handle, uint64 init) {
    mySemaphore** myHandle = (mySemaphore**)handle;
    *myHandle = mySemaphore::createSem(init);
}
void sem_closeWrapper(uint64 handle) {
    mySemaphore* myHandle = (mySemaphore*)handle;
    myHandle->setDealoc(true);
    int i = myHandle->getValue();
    for (;i <= 0; i++) myHandle->signal();
    delete myHandle;
}
void sem_waitWrapper(uint64 handle) {
    mySemaphore* myHandle = (mySemaphore*)handle;
    myHandle->wait();
}
void sem_signalWrapper(uint64 handle) {
    mySemaphore* myHandle = (mySemaphore*)handle;
    myHandle->signal();
}
void time_sleepWrapper(uint64 time) {
    time_t t = (time_t) time;
    TCB::putToSleep(t);
}
char getcWrapper() {
    return myConsole::getcIn();
}
void putcWrapper(char ch) {
    myConsole::putcOut(ch);
}
void Riscv::popA0(){
    uint64 scause = r_scause();
    if (!(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)){
        __asm__ volatile ("ld x10, 10 * 8(s0)");
    }
}
void Riscv::wakeSleepingThreads() {
    if (TCB::sleepHead) {
        if (TCB::sleepHead->sleepLeft == 0) {
            TCB::sleepHead->setSleeping(false);
            Scheduler::put(TCB::sleepHead);
            TCB* tmp = TCB::sleepHead;
            TCB::sleepHead = TCB::sleepHead->nextSleep;
            tmp->nextSleep = nullptr;
            wakeSleepingThreads();
        }
    }
}

void Riscv::handleSupervisorTrap()
{
    uint64 a0 = r_a0();
    uint64 a1 = r_a1viaframe();
    uint64 a2 = r_a2viaframe();
    uint64 a3 = r_a3viaframe();
    uint64 a4 = r_a4viaframe();
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        switch (a0) {
            case mem_alloc: mem_allocWrapper(a1); break;
            case mem_free: mem_freeWrapper(a1); break;
            case thread_create: thread_createWrapper(a1, a2, a3, a4); break;
            case thread_exit: thread_exitWrapper(); break;
            case thread_start: thread_startWrapper(a1); break;
            case thread_create_no_start: thread_create_no_startWrapper(a1, a2, a3, a4); break;
            case sem_open: sem_openWrapper(a1,a2); break;
            case sem_close: sem_closeWrapper(a1); break;
            case sem_wait: sem_waitWrapper(a1); break;
            case sem_signal: sem_signalWrapper(a1); break;
            case time_sleep: time_sleepWrapper(a1); break;
            case getc: getcWrapper(); break;
            case putc: putcWrapper((char)a1); break;
        }
        __asm__ volatile ("addi sp, sp, -16");
        __asm__ volatile ("sd x10, (sp)");
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();
        if (Scheduler::head && a0 != sem_wait && a0 != sem_signal && a0 != time_sleep && a0 != putc) {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        __asm__ volatile ("ld x10, (sp)");
        __asm__ volatile ("addi sp, sp, 16");
        w_sstatus(sstatus);
        w_sepc(sepc);
        w_scause(scause);
    }else if(scause == 0x8000000000000001UL){
        // interrupt from timer (supervisor software interrupt)
        TCB::timeSliceCounter++;
        if(TCB::sleepHead)TCB::sleepHead->sleepLeft--;
        wakeSleepingThreads();
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            if (Scheduler::head) TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);

        }
        mc_sip(SIP_SSIP);
    }else if (scause == 0x8000000000000009UL){
        uint64 sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        //  console interrupt (supervisor external interrupt)
        if (plic_claim() == consoleIRQ){

            char status = *(char*)CONSOLE_STATUS;
            while (CONSOLE_RX_STATUS_BIT & status){
                char ch = *(char*) CONSOLE_RX_DATA;
                myConsole::putcIn(ch);
                status = *(char*)CONSOLE_STATUS;
            }
        }
        plic_complete(CONSOLE_IRQ);
        w_sstatus(sstatus);
        w_sepc(sepc);
    } else {
        uint64 sepc = r_sepc();
        // unexpected trap cause
        printString("scause: ");
        printInt(scause);
        printString("\n");

        printString("sepc: ");
        printInt(sepc);
        printString("\n");
    }
}

