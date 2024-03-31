
; Linker symbols resolve during the linking stage not in here! 


; __do_global_ctors and __do_global_dtors necessary if there are any constructors/destructors. 

.section .init8,"ax",@progbits
.global __do_global_ctors
__do_global_ctors:
    ldi r30, lo8(__ctors_start)
    ldi r31, hi8(__ctors_start)
    ldi r24, lo8(__ctors_end)
    ldi r25, hi8(__ctors_end)
ctor_loop:
    cpi r30, lo8(__ctors_end)
    cpc r31, r25
    breq end_ctor_loop  ; If __ctors_start == __ctors_end, done
    icall               ; Call constructor
    adiw r30, 2         ; Move to next constructor (2 bytes per pointer on AVR)
    rjmp ctor_loop
end_ctor_loop:
    ret 


#define __zero_reg__ r1


.section .fini8,"ax",@progbits
.global __do_global_dtors
__do_global_dtors:
    ; Destructors need to be called in reverse order
    ldi r28, lo8(__dtors_end)  ; Setup Z-pointer (r30:r31) to end of dtors
    ldi r29, hi8(__dtors_end) 
    ldi r26, lo8(__dtors_start) ; Setup X-pointer (r26:r27) to start of dtors
    ldi r27, hi8(__dtors_start)
dtor_loop:
    sbiw r28, 2     ; Move to the previous destructor (2 bytes per pointer on AVR)
    cp  r26, r28    ; Compare pointers to see if we've reached the start
    cpc r27, r29    
    breq end_dtor_loop ; Exit loop if start is reached
    icall           ; Call destructor
    rjmp dtor_loop
end_dtor_loop:
    ret


.section .text

.global __bad_interrupt
.func __bad_interrupt
__bad_interrupt:
	rjmp .  
.endfunc


.section .init0, "ax", @progbits
.global __init
.func __init
__init:

; First 256 bytes of SRAM reserved for the 32 general-purpose registers, I/O registers and extended I/O register.
.weak __sram_base
.set __sram_base, 0x800100

.weak __sram_end
.set __sram_end, 0x8008ff

; 2KB SRAM (0x8008ff - 0x800100)


; ********** Initialize Stack Pointer **********
.weak __stack
.set __stack, __sram_end  ; Define stack end, RAMEND

ldi r16, lo8(__stack)    ; Load low byte of stack end (0x08FF) into r16
out 0x3D, r16             ; SPL address
ldi r16, hi8(__stack)    ; Load high byte of stack end (0x08FF) into r16
out 0x3E, r16             ; SPH address

clr __zero_reg__
out 0x3F, __zero_reg__


rjmp __do_copy_data       ; Jump to data copy routine



; ********** Copy data & clear bss **********

; Copy Data section into SRAM
.section .init4, "ax", @progbits
__do_copy_data:
    ldi r26, lo8(__data_start)        ; Initialize X pointer to start of .data in SRAM
    ldi r27, hi8(__data_start)
    ldi r30, lo8(__data_load_start)   ; Initialize Z pointer to start of .data in flash
    ldi r31, hi8(__data_load_start)
    ldi r18, lo8(__data_end)          ; Load address bounds of .data section
    ldi r19, hi8(__data_end)
copy_loop:
    lpm r0, Z+                        ; Load a byte from flash to r0 and increment Z
    st X+, r0                         ; Store the byte from r0 into SRAM and increment X
    cpi r26, lo8(__data_end)          ; Compare low byte of X pointer against low byte of data_end
    cpc r27, r19                      ; Compare with carry, high byte of X pointer against high byte of data_end
    brcc copy_done                    ; If carry cleared, reached or passed __data_end
    rjmp copy_loop                    ; Continue loop if not done

copy_done:
    rjmp __init9
    

.section .init9,"ax",@progbits
__init9:
    rcall __do_global_ctors  
    rcall main
    rcall __do_global_dtors  

exit:
    rjmp exit

.endfunc


.section .vectors,"ax",@progbits
.global __vectors
.func   __vectors
__vectors:
    jmp __init
    jmp __bad_interrupt
.endfunc
