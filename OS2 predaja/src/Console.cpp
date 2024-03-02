//
// Created by os on 8/24/22.

#include "../h/syscall_cpp.h"


char Console::getc() {
    return ::getc();
}

void Console::putc(char ch) {
    ::putc(ch);
}