//
// Created by os on 8/22/22.
//

#ifndef PROJECT_BASE_MYCONSOLE_H
#define PROJECT_BASE_MYCONSOLE_H
#include "myBuffer.h"


class myConsole {
public:

    static void console_init();

    static char getcIn();
    static void putcIn(char);

    static char getcOut();
    static void putcOut(char);

private:
    static myBuffer* buffIn;
    static myBuffer* buffOut;
    myConsole() {};

};


#endif //PROJECT_BASE_MYCONSOLE_H
