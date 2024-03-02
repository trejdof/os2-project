//
// Created by os on 8/22/22.
//

#include "../h/myConsole.h"

myBuffer* myConsole::buffIn = nullptr;
myBuffer* myConsole:: buffOut = nullptr;

void myConsole::console_init() {
    buffIn = new myBuffer(256);
    buffOut = new myBuffer(256);
}

char myConsole::getcIn() {
    return buffIn->get();
}

void myConsole::putcIn(char ch) {
    buffIn->put(ch);
}

char myConsole::getcOut() {
    return buffOut->get();
}
void myConsole::putcOut(char ch) {
    buffOut->put(ch);
}

void consoleConsumerBody(){
    while (true){
        char status = *(char*)CONSOLE_STATUS;
        while(status &  CONSOLE_TX_STATUS_BIT) {
            *(char *) CONSOLE_TX_DATA = myConsole::getcOut();
            status = *(char *) CONSOLE_STATUS;
        }

    }
};
