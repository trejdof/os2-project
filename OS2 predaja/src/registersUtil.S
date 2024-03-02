.global _ZN5Riscv13pushRegistersEv
.type _ZN5Riscv13pushRegistersEv, @function

//Push registers including a0
_ZN5Riscv13pushRegistersEv:
    addi sp, sp, -256
    .irp index, 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr
    ret



.global _ZN5Riscv12popRegistersEv
.type _ZN5Riscv12popRegistersEv, @function

//Pop registers including a0
_ZN5Riscv12popRegistersEv:
    .irp index, 5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr
    addi sp, sp, 256
    ret



// extern "C" void pushRegisters();
// extern "C" void popRegisters();



