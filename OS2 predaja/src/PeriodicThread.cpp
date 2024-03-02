//
// Created by os on 9/1/22.
//
#include "../h/syscall_cpp.h"


void PeriodicThread::wrapper(void *arg) {
    periodic_struct* st = (periodic_struct*)arg;
    while(true){
        time_sleep(st->period);
        st->pt->periodicActivation();
    }
}
