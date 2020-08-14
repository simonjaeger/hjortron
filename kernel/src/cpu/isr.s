section .text

%macro ISR_DEFAULT 1
global isr%1
isr%1:
    cli
    push 0
    push %1
    jmp isr_common
%endmacro

%macro ISR_ERROR_CODE 1
global isr%1
isr%1:
    cli
    push %1
    jmp isr_common
%endmacro

ISR_DEFAULT     0   ; Divide by zero.
ISR_DEFAULT     1   ; Debug.
ISR_DEFAULT     2   ; Non-maskable interrupt.
ISR_DEFAULT     3   ; Breakpoint.
ISR_DEFAULT     4   ; Overflow.
ISR_DEFAULT     5   ; Out of bounds.
ISR_DEFAULT     6   ; Invalid opcode.
ISR_DEFAULT     7   ; Device not available.
ISR_ERROR_CODE  8   ; Double fault.
ISR_DEFAULT     9   ; Reserved.
ISR_ERROR_CODE  10  ; Invalid TSS.
ISR_ERROR_CODE  11  ; Invalid segment.
ISR_ERROR_CODE  12  ; Stack fault.
ISR_ERROR_CODE  13  ; General protection fault.
ISR_ERROR_CODE  14  ; Page fault.
ISR_DEFAULT     15  ; Reserved
ISR_DEFAULT     16  ; x87 floating-point.
ISR_ERROR_CODE  17  ; Alignment check.
ISR_DEFAULT     18  ; Machine check.
ISR_DEFAULT     19  ; SIMD floating-point.
ISR_DEFAULT     20  ; Virtualization.
ISR_DEFAULT     21  ; Reserved.
ISR_DEFAULT     22  ; Reserved.
ISR_DEFAULT     23  ; Reserved.
ISR_DEFAULT     24  ; Reserved.
ISR_DEFAULT     25  ; Reserved.
ISR_DEFAULT     26  ; Reserved.
ISR_DEFAULT     27  ; Reserved.
ISR_DEFAULT     28  ; Reserved.
ISR_DEFAULT     29  ; Reserved.
ISR_ERROR_CODE  30  ; Security.
ISR_DEFAULT     31  ; Reserved.

ISR_DEFAULT     32  ; Timer.
ISR_DEFAULT     33  ; Keyboard.
ISR_DEFAULT     34  ; Cascade (PIC).
ISR_DEFAULT     35  ; COM2.
ISR_DEFAULT     36  ; COM1.
ISR_DEFAULT     37  ; LTP2;
ISR_DEFAULT     38  ; FDD.
ISR_DEFAULT     39  ; LTP1.
ISR_DEFAULT     40  ; CMOS.
ISR_DEFAULT     41  ; Peripherals.
ISR_DEFAULT     42  ; Peripherals.
ISR_DEFAULT     43  ; Peripherals.
ISR_DEFAULT     44  ; Mouse.
ISR_DEFAULT     45  ; FPU.
ISR_DEFAULT     46  ; Primary ATA HDD.
ISR_DEFAULT     47  ; Secondary ATA HDD.

ISR_DEFAULT     128  ; System call.

isr_common:
    ; Push state.
    pushad
    push ds
    push es
    push fs
    push gs

    ; Push stack address.    
    mov eax, esp
    push eax

    ; Call handler.
    extern irq_handler
    mov eax, irq_handler
    call eax

    ; Restore state.
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8

global isr_ignore 
isr_ignore:
    iret
