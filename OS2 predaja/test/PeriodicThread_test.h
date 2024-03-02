//
// Created by os on 9/1/22.
//

#ifndef PROJECT_BASE_V1_1_PERIODICTHREAD_TEST_H
#define PROJECT_BASE_V1_1_PERIODICTHREAD_TEST_H
#include "../h/syscall_cpp.h"
#include "printing.hpp"

class Time : public PeriodicThread {
public:
    Time(time_t period) : PeriodicThread(period) { second = 0; id = counter++;}

    void periodicActivation() override {
        printInt(id);
        printString(" ");
    }

private:
    int second;
    static int counter;
    int id;
};
int Time::counter = 0;
void periodicThreadTest() {

    Time *timeThread = new Time(10);
    Time* timeThread1 = new Time (20);


    timeThread->start();
    timeThread1->start();

}

#endif //PROJECT_BASE_V1_1_PERIODICTHREAD_TEST_H
