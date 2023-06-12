// ; startup code to support HLL programs
// ; snatched from $RV_ROOT/testbench/asm/crt0.s
#include "defines.h"

.section .text.init
.global _start
_start:
    la sp, STACK
    call main

.global _finish
_finish:
        la t0, tohost
        sd a0, 0(t0)
        beq x0, x0, _finish
        .rept 10
        nop
        .endr


.section .tohost
.global tohost
tohost: .dword 0
.size tohost, 8
.global fromhost
fromhost: .dword 0
.size fromhost, 8