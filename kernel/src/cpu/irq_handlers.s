section .text

%macro IRQ_DEFAULT 1
global irq%1
irq%1:
    mov byte [number], %1       ; Save interrupt number for push.
    jmp irq_common
%endmacro

%macro IRQ_ERROR_CODE 1
global irq%1
irq%1:
    mov byte [number], %1       ; Save interrupt number for push.
    jmp irq_common
%endmacro

; Exception IRQ's.
IRQ_DEFAULT     0x00    ; Divide-by-zero error.
IRQ_DEFAULT     0x01    ; Debug.
IRQ_DEFAULT     0x02    ; Non-maskable interrupt.
IRQ_DEFAULT     0x03    ; Breakpoint.
IRQ_DEFAULT     0x04    ; Overflow.
IRQ_DEFAULT     0x05    ; Bound range exceeded.
IRQ_DEFAULT     0x06    ; Invalid opcode.
IRQ_DEFAULT     0x07    ; Device not available.
IRQ_ERROR_CODE  0x08    ; Double fault.
IRQ_ERROR_CODE  0x0A    ; Invalid TSS.
IRQ_ERROR_CODE  0x0B    ; Segment not present
IRQ_ERROR_CODE  0x0C    ; Stack-segment fault.
IRQ_ERROR_CODE  0x0D    ; General protection fault.
IRQ_ERROR_CODE  0x0E    ; Page fault.
IRQ_DEFAULT     0x10    ; x87 floating-point exception.
IRQ_ERROR_CODE  0x11    ; Alignment check.
IRQ_DEFAULT     0x12    ; Machine check.
IRQ_DEFAULT     0x13    ; SIMD floating-point exception.
IRQ_DEFAULT     0x14    ; Virtualization exception.
IRQ_ERROR_CODE  0x1E    ; Security exception.

; PIC IRQ's.
IRQ_DEFAULT 0x20    ; Timer.
IRQ_DEFAULT 0x21    ; Keyboard.
IRQ_DEFAULT 0x22    ; Cascade (PIC).
IRQ_DEFAULT 0x23    ; COM2.
IRQ_DEFAULT 0x24    ; COM1.
IRQ_DEFAULT 0x25    ; LTP2;
IRQ_DEFAULT 0x26    ; FDD.
IRQ_DEFAULT 0x27    ; LTP1.
IRQ_DEFAULT 0x28    ; CMOS.
IRQ_DEFAULT 0x29    ; Peripherals.
IRQ_DEFAULT 0x2A    ; Peripherals.
IRQ_DEFAULT 0x2B    ; Peripherals.
IRQ_DEFAULT 0x2C    ; Mouse.
IRQ_DEFAULT 0x2D    ; FPU.
IRQ_DEFAULT 0x2E    ; Primary ATA HDD.
IRQ_DEFAULT 0x2F    ; Secondary ATA HDD.

irq_common:
    pushad                      ; Push state.
	push ds
	push es
	push fs
	push gs 

    mov eax, [number]           ; Load interrupt number.
    push eax                    ; Push interrupt number.
    extern irq_handle
    call irq_handle

    pop eax                     ; Restore stack.
	pop gs                      ; Pop state.
	pop fs
	pop es
	pop ds
    popad
    iret

global irq_ignore:              ; Catcher for unhandled interrupts.
irq_ignore:
    iret

.data:
    number: db 0

