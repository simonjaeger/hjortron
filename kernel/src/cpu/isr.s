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

ISR_DEFAULT     0   ; Divide-by-zero error.
ISR_DEFAULT     1   ; Debug.
ISR_DEFAULT     2   ; Non-maskable interrupt.
ISR_DEFAULT     3   ; Breakpoint.
ISR_DEFAULT     4   ; Overflow.
ISR_DEFAULT     5   ; Bound range exceeded.
ISR_DEFAULT     6   ; Invalid opcode.
ISR_DEFAULT     7   ; Device not available.
ISR_ERROR_CODE  8   ; Double fault.
ISR_ERROR_CODE  10  ; Invalid TSS.
ISR_ERROR_CODE  11  ; Segment not present
ISR_ERROR_CODE  12  ; Stack-segment fault.
ISR_ERROR_CODE  13  ; General protection fault.
ISR_ERROR_CODE  14  ; Page fault.
ISR_DEFAULT     16  ; x87 floating-point exception.
ISR_ERROR_CODE  17  ; Alignment check.
ISR_DEFAULT     18  ; Machine check.
ISR_DEFAULT     19  ; SIMD floating-point exception.
ISR_DEFAULT     20  ; Virtualization exception.
ISR_ERROR_CODE  30  ; Security exception.

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
