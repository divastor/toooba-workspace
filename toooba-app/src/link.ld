/*
 NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE NOTE
 This is an automatically generated file by stavros on Τετ 02 Μαρ 2022 07:35:18 μμ EET

 cmd:    swerv -snapshot=swerv-app -unset=assert_on -set=reset_vec=0x80000000 -set=fpga_optimize=1 

*/

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init .  : { *(.text.init) }
  
  .text . : { *(.text) }
  _end = .;
    . = 0x80400000 ;
  .bss :  {BSS_START = .; *(.*bss)}
  BSS_END = .;
  . = ALIGN(0x1000);
  .data  :  ALIGN(0x1000) { *(.*data) *(.rodata*)}
  .tohost .  : { *(.data.io) }
  STACK = ALIGN(16) + 0x1000;
  
    . = 0x80fffff8; .data.ctl : { LONG(0x80400000); LONG(STACK) }
}

