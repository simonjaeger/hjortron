section .text

global scheduler_iret
scheduler_iret:

    pop gs
    pop fs
    pop es
    pop ds
    popad
    add esp, 8

    iret
    



    