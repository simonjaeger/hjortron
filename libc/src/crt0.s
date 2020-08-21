section .text

global _start

_start:
    extern main
    call main

    mov eax, edi
    xor eax, eax

    extern exit
    call exit