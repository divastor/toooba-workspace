#include "defines.h"

.section .data
.global control
control:
.byte  0xff
.rept 12
.word  MAGIC_BYTE
.endr
unused_before:
.rept 64
.dword 0xffffffff
.endr
.global secret
secret:     .string "PASSWORD123\0"
unused_after:
.rept 64
.dword 0xffffffff
.endr