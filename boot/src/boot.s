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

start:
    cli

    xor ax, ax    
    mov ds, ax
    mov es, ax

    ; Set up stack.
    mov ss, ax
    mov sp, 0x7C00

    ; Read root directory.
    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, 0x9000              ; Destination offset.
    mov si, 19                  ; First sector of root directory.
    mov al, 14                  ; Number of sectors.
    ; mov dl, dl                ; Drive "should" be set by BIOS.
    call read16




    mov cx, [directory_entries]
    mov ax, 0x9000

.loop:

    push cx

    mov cx, 11
    mov si, File
    mov di, ax

    repe cmpsb

    jnz .cont
    
    mov si, ax
    call puts

.cont:

    pop cx


    add ax, 0x20
    dec cx
    jnz .loop


    ; Print.
    ; mov si, SystemIdentifier
    ; call puts




halt:
    jmp halt
    hlt


    File db "TEST    BIN"



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