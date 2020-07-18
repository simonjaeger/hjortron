section .boot
bits 16

global boot
boot:
    ; BIOS parameter block.
    jmp short start
    nop

    oem_identifier                      dq 0
    bytes_per_sector                    dw 0
    sectors_per_cluster                 db 0
    reserved_sectors                    dw 0
    file_allocation_tables              db 0
    directory_entries                   dw 0
    total_sectors                       dw 0
    media_descriptor_type               db 0
    sectors_per_file_allocation_table   dw 0
    sectors_per_track                   dw 0
    heads_per_cylinder                  dw 0
    hidden_sectors                      dd 0
    large_sectors                       dd 0

    ; Extended boot record (FAT12).
    drive_number                        db 0
    reserved                            db 0
    signature                           db 0
    volume_identifier                   dd 0
    volume_label                        times 11 db 0
    system_identifier                   dq 0

; TODO: Use BPB to compute values.
FAT1_SEGMENT equ 0x7E00
FAT2_SEGMENT equ FAT1_SEGMENT + 512 * 9
ROOT_SEGMENT equ FAT2_SEGMENT + 512 * 9
DATA_SEGMENT equ ROOT_SEGMENT + 512 * 14

start:
    cli

    xor ax, ax    
    mov ds, ax
    mov es, ax

    ; Set up stack.
    mov ss, ax
    mov sp, 0x7C00

    ; Read FAT1.
    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, FAT1_SEGMENT        ; Destination offset.
    mov si, 1                   ; First sector.
    mov al, 9                   ; Number of sectors.
    ; mov dl, dl                ; Drive "should" be set by BIOS.
    call read16

    ; Read root directory.
    xor ax, ax
    mov bx, ROOT_SEGMENT        ; Destination offset.
    mov si, 19                  ; First sector.
    mov al, 14                  ; Number of sectors.
    ; mov dl, dl                ; Drive "should" be set by BIOS.
    call read16






    mov cx, [directory_entries]
    mov di, ROOT_SEGMENT

.loop:
    push cx

    mov cx, 11
    mov si, test_file
    ; mov di, ax

    repe cmpsb
    jnz .cont

    ; File found...
    mov si, file_found
    call puts


    ; add di, 0x1A
    mov si, [di-11+0x1A]    ; Get cluster.
    ; dec si

    sub si, 2
    add si, 1+9+9+14

    ; Read test.
    xor ax, ax
    mov bx, 0x9000              ; Destination offset.

    mov al, 1                  ; Number of sectors.
    ; mov dl, dl                ; Drive "should" be set by BIOS.
    call read16



    ; dec ax
    ; ; mov bx, 12
    ; ; mul bx
    ; mov ax, FAT1_SEGMENT
    ; add ax, 0

    ; mov di, ax
    ; mov ax, [di]
    
    ; ; mov si, [FAT1_SEGMENT + si*12]
    ; ; mov si, 0x5445

    ; ; cmp ax, 0x0003
    ; and ax, 0000111111111111b
    ; cmp ax, 0x0FFF
    ; jne .cont

    mov si, 0x9000
    ; mov si, file_found
    call puts

.cont:

    pop cx

    add di, 0x20
    dec cx
    jnz .loop


    ; Print.
    ; mov si, SystemIdentifier
    ; call puts




halt:
    jmp halt
    hlt


    test_file db "TEST    BIN"
    file_found db "Found file!", 0



; puts
; Print string using teletype output.
;
; si = string

puts:
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

; read16
; Read sectors from disk to memory.
;
; es:bx = destination
; si    = lba
; al    = number of sectors
; dl    = drive

read16:
    pusha

    mov ah, 0x02                ; Read sectors.
    push ax                     ; Store number of sectors.
    push dx                     ; Store drive.

    mov ax, si                  ; Load LBA.

    ; Convert LBA to CHS.
    ; C = (LBA / Sectors) / Heads
    ; H = (LBA / Sectors) % Heads
    ; S = (LBA % Sectors) + 1
    xor dx, dx
    div word [sectors_per_track]
    mov cl, dl  
    inc cl
    xor dx, dx
    div word [heads_per_cylinder]
    mov dh, dl
    mov ch, al
    shl ah, 6 
    or cl, ah

    pop ax                      ; Restore drive.
    mov dl, al
    pop ax                      ; Restore number of sectors.

    int 0x13                    ; Read sectors.

    ; TODO: Check status and handle error.

    popa
    retn