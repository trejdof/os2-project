//
// Created by os on 6/26/22.
//

#include "../h/myScheduler.h"
#include "../h/tcb.h"
#include "../h/riscv.h"
#include "../h/syscall_c.h"

extern TCB* idle;
TCB* Scheduler::head = nullptr;
TCB* Scheduler::tail = nullptr;

TCB *Scheduler::get() {
    if(head == nullptr){
        tail=nullptr;
        return idle;
    }
    TCB* tmp = head; //
    head = head->nextTCB;
    tmp->nextTCB = nullptr;
    return tmp;
}

void Scheduler::put(TCB *tcb) {
    if (tcb->id == idleThread) return;
    if(head != nullptr) {
        tail->nextTCB = tcb;
        tail = tcb;
    }else{
        head=tcb;
        tail=tcb;
    }
}
void idleThreadBody(){
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
    while (true) thread_dispatch();
}