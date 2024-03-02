//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP

#include "../lib/hw.h"

class Riscv
{
public:
    // push x3..x31 registers onto stack
    static void pushRegisters();

    //pop x3..x31 registers from stack
    static void popRegisters();


    static void popA0();
    static void wakeSleepingThreads();
    static void setPrivilege();
    static void pushNeededRegisters();

    static void popNeededRegisters();

    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();

    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip
    {
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);
    enum BitMaskSie
    {
        SIE_SSIE = (1 << 1),
        SIE_SEIE = (1 << 9),
    };
    // mask set register sie
    static void ms_sie(uint64 mask);

    // mask clear register sie
    static void mc_sie(uint64 mask);


    enum BitMaskSstatus
    {
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // supervisor trap
    static void supervisorTrap();

    // read register a0( system call code);
    static uint64 r_a0();

    // read register a1 (1st argument)
    static uint64 r_a1viaframe();

    static uint64 r_a2viaframe();
    static uint64 r_a3viaframe();
    static uint64 r_a4viaframe();


    // write in a0 ( return value)
    static void w_a0(uint64 value);

    enum someCodes
    {
        mem_alloc = 0x1,
        mem_free = 0x2,
        thread_create = 0x11,
        thread_exit = 0x12,
        thread_dispatch = 0x13,
        thread_start = 0x14,
        thread_create_no_start = 0x15,
        sem_open = 0x21,
        sem_close = 0x22,
        sem_wait = 0x23,
        sem_signal = 0x24,
        time_sleep = 0x31,
        getc = 0x41,
        putc = 0x42,
        consoleIRQ = 0x0a
    };
protected:

    static void handleSupervisorTrap();

};


inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline uint64 Riscv::r_a0() {
    uint64 volatile a0;
    __asm__ volatile("mv %0, a0" : "=r" (a0));
    return a0;
}

inline uint64 Riscv::r_a1viaframe() {
    uint64 volatile a1;
    __asm__ volatile("ld %0, 88(s0)" : "=r" (a1));
    return a1;
}
inline uint64 Riscv::r_a2viaframe() {
    uint64 volatile a2;
    __asm__ volatile("ld %0, 96(s0)" : "=r" (a2));
    return a2;
}
inline uint64 Riscv::r_a3viaframe() {
    uint64 volatile a3;
    __asm__ volatile("ld %0, 104(s0)" : "=r" (a3));
    return a3;
}
inline uint64 Riscv::r_a4viaframe() {
    uint64 volatile a4;
    __asm__ volatile("ld %0, 112(s0)" : "=r" (a4));
    return a4;
}


inline void Riscv::w_a0(uint64 value) {
    __asm__ volatile ("mv a0, %0" : "=r" (value));
}
inline void Riscv::ms_sie(uint64 mask)
{
    __asm__ volatile ("csrs sie, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sie(uint64 mask)
{
    __asm__ volatile ("csrc sie, %[mask]" : : [mask] "r"(mask));
}


#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
