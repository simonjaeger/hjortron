section .text
bits 16

; print16
; Print string using teletype output.
;
; Input:
; si = string

global print16
print16:
    pusha
    cld                         ; Clear direction flag.
    mov bx, 0x07                ; Clear page and color.
    mov ah, 0x0E                ; Teletype output.

.loop:
    lodsb                       ; Load character.
    or al, al                   ; Check for null terminator.
    jz .end 
    int 0x10 
    jmp .loop                   ; Go to next character.

.end:
    popa
    retn