section .text

%macro IRQ 1
global irq%1
irq%1:
    mov byte [number], %1       ; Save interrupt number for push.
    jmp irq_common
%endmacro

IRQ 0x20
IRQ 0x21
IRQ 0x22
IRQ 0x23
IRQ 0x24
IRQ 0x25
IRQ 0x26
IRQ 0x27
IRQ 0x28
IRQ 0x29
IRQ 0x2A
IRQ 0x2B
IRQ 0x2C
IRQ 0x2D
IRQ 0x2E
IRQ 0x2F

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

