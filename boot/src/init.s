section .text
bits 16

global init
init:

    extern print
    mov si, LABEL
    call print

halt:
    jmp halt
    hlt

db "Sector0", 0

TIMES 512 - ($ - $$) db 0

db "Sector1", 0

TIMES 1024 - ($ - $$) db 0

db "Sector2", 0

LABEL            db "Hello from TEST.BIN!", 0

