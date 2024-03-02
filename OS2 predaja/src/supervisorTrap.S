.extern _ZN5Riscv20handleSupervisorTrapEv

.align 4
.global _ZN5Riscv14supervisorTrapEv
.type _ZN5Riscv14supervisorTrapEv, @function
_ZN5Riscv14supervisorTrapEv:
    #push ra so it can be restored on a different stack
    addi sp, sp, -16
    sd ra, 0(sp)
    #x4 and x5 registers are being used by switchToSystemStack function
    #so we save them and pop them inside function using frame pointer s0
    addi sp, sp, -8
    sd x14, 0(sp)
    addi sp,sp, -8
    sd x15, 0(sp)

    # switch to system stack for kernel code
    call _ZN3TCB19switchToSystemStackEv

    # push all registers to stack
    addi sp, sp, -256
    .irp index, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _ZN5Riscv20handleSupervisorTrapEv

    #pop a0 if needed (ecall from user and privilaged mode)
    call _ZN5Riscv5popA0Ev

    # pop all registers from stack
    .irp index, 0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256

    # switch to user stack when exiting ecall
    call _ZN3TCB17switchToUserStackEv

    #alligning stack due to a4,a5
    addi sp,sp, 16

    #restoring ra
    ld ra, 0(sp)
    addi sp, sp, 16

    sret