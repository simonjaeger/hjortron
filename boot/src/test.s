section .text
bits 16

global text
text:

db "Sector0", 0

TIMES 512 - ($ - $$) db 0

db "Sector1", 0

TIMES 1024 - ($ - $$) db 0

db "Sector2", 0