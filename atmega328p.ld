

MEMORY
{
  text (rx) : ORIGIN = 0, LENGTH = 256K
  data (rw) : ORIGIN = 0x800100, LENGTH = 2K
}

SECTIONS
{
  .text :
  {
    *(.vectors)
    *(.init0) *(.init1) *(.init2) *(.init3)
    *(.init4) *(.init5) *(.init6) *(.init7)
    *(.init8) *(.init9)
    *(.text) *(.text.*)
    *(.rodata) *(.rodata*)  /* Place .rodata in program memory */
    . = ALIGN(2);
    _etext = .;             /* Define a global symbol marking the end of .text */
  } > text

    .data : AT (_etext) {
        __data_start = .; /* Provide symbol for start of .data */
        *(.data)
        . = ALIGN(4); 
        __data_end = .; /* Provide symbol for end of .data */
    } > data

        .bss : {
        __bss_start = .; /* Provide symbol for start of .bss */
        *(.bss)
            *(COMMON)
        . = ALIGN(4); 
        __bss_end = .; /* Provide symbol for end of .bss */
    } > data

        /* Define symbols for the start and end of the loaded .data section in flash */
    __data_load_start = LOADADDR(.data);
    __data_load_end = .; 

}
