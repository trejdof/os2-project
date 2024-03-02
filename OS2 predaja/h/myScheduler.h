//
// Created by os on 6/26/22.
//

#ifndef PROJECT_BASE_SCHEDULER_H
#define PROJECT_BASE_SCHEDULER_H


class TCB;

class Scheduler {
public:
    static TCB* head;
    static TCB* tail;

public:
    static TCB *get();
    static void put(TCB *tcb);
};


#endif //PROJECT_BASE_SCHEDULER_H
