section .text
bits 32

global test
test:
    ; Print name.
    mov esi, hello
    extern print32
    call print32

halt:
    jmp halt
    hlt

data:
    hello: db "Hello!", 0