target remote :1234
info functions
b main
c
b USART_Init
c
info stack
info all-registers
info mem
info address buffer
info address global_value
info address __sram_base
info address __sram_end
info address __stack
info variables
info registers sp
layout asm
x/2048xb 0x800000