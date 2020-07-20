section .text
bits 32

; print32
; Print a string on the screen.
;
; Input:
; esi = address of string

global print32
print32:
    pusha
    mov ebx, 0xB8000

.loop:
    lodsb
    or al, al
    jz .end
    or eax, 0x0700
    mov word [ebx], ax
    add ebx, 2
    jmp .loop

.end:
    popa
    retn