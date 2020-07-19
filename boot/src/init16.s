section .text
bits 16

global init16
init16:
    ; Load from boot.
    pop di
    mov dx, [di]
    extern boot_drive
    mov byte [boot_drive], dl

    ; Enable A20 address line.
    int 0x15    
    
    ; Set VGA text mode 3.
    mov ax, 0x3
    int 0x10            

    ; Disable cursor.
    mov ah, 0x01           
    mov ch, 0x3F
    int 0x10

    ; Read memory map.
    call read_e820
    
    ; Load GDT.
    cli                         
    lgdt [GDT]    

    ; Enable protected mode.
    mov eax, cr0                ; Set lowest bit of cr0.
	or eax, 0x1
	mov cr0, eax

    extern init32
    jmp 0x8:init32              ; Far jump to load CS register.

error:
    extern print16
    call print16

halt:
    jmp halt
    hlt

data:
    ; Set up initial global descriptor table
    ; for protected mode.
    GDT_NULL:
        dq 0x0
    GDT_CODE:
        dw 0xFFFF, 0
        db 0, 0x9A, 0xCF, 0
    GDT_DATA:
        dw 0xFFFF, 0
        db 0, 0x92, 0xCF, 0
    global GDT
    GDT:
        dw GDT - GDT_NULL - 1   ; Size.
        dd GDT_NULL             ; Offset.

    ; Strings.
    ERROR_E820  db "Could not read memory map.", 0

; read_e820
; Detect memory map using E820 function.

SMAP equ 0x0534D4150

read_e820:
    pusha

    ; Reset registers.
    xor ebx, ebx
    xor bp, bp                  ; Counter.

    ; Set destination.
    xor ax, ax
    mov es, ax
    extern mmap_entries
    mov di, [mmap_entries]

.loop:
    mov edx, SMAP               ; Set to "SMAP".
    mov eax, 0xE820             ; Set function.

    mov ecx, 24                 ; Read 24 bytes.
    int 0x15
    jc short .error             ; Check for error (unsupported).

    mov edx, SMAP               ; Set to "SMAP" again.
    cmp eax, edx                ; Expect "SMAP" on success.
    jne short .error

    inc bp                      ; Increment counter.

    cmp ebx, 0                  ; Check for no more data.
    je .end

    ; TODO: Skip "empty" entries (length = 0).

    add di, 24                  ; Increment destination.
    jmp .loop

.error:
    mov si, ERROR_E820
    jmp error

.end:
    extern mmap_count
    mov [mmap_count], bp        ; Save counter.
    clc                         ; Clear carry.

    popa
    retn