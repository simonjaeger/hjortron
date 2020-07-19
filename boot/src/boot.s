section .text
bits 16

global boot
boot:
    ; BIOS parameter block.
    jmp short start
    nop

    OEM_IDENTIFIER          dq 0
    BYTES_PER_SECTOR        dw 0
    SECTORS_PER_CLUSTER     db 0
    RESERVED_SECTORS        dw 0
    FATS                    db 0
    DIRECTORY_ENTRIES       dw 0
    SECTORS                 dw 0
    MEDIA_DESCRIPTOR_TYPE   db 0
    SECTORS_PER_FAT         dw 0
    SECTORS_PER_TRACK       dw 0
    HEADS_PER_CYLINDER      dw 0
    HIDDEN_SECTORS          dd 0
    LARGE_SECTORS           dd 0

    ; Extended boot record (FAT12).
    DRIVE_NUMBER        db 0
    RESERVED            db 0
    SIGNATURE           db 0
    VOLUME_IDENTIFIER   dd 0
    VOLUME_LABEL        times 11 db 0
    SYSTEM_IDENTIFIER   dq 0

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
    mov dl, byte [boot_drive]   ; Drive.
    call read16

    ; Read root directory.
    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, ROOT_DIRECTORY_SEGMENT ; Destination offset.
    mov si, 1+9+9               ; First sector.
    mov al, 14                  ; Number of sectors.
    mov dl, byte [boot_drive]   ; Drive.
    call read16

    ; Find INIT.BIN.
    mov si, FILE_INIT_BIN
    call find_file

    ; Load INIT.BIN.
    mov ax, [di+0x1A]
    mov bx, INIT_SEGMENT
    call read_file

    ; Find KERNEL.BIN.
    mov si, FILE_KERNEL_BIN
    call find_file

    ; Load KERNEL.BIN.
    mov ax, 0x1000
    mov es, ax
    mov ax, [di+0x1A]
    mov bx, 0
    call read_file

    ; Jump to INIT.BIN.
    jmp INIT_SEGMENT

error:
    extern print16
    call print16

halt:
    jmp halt
    hlt

data:
    boot_drive db 0
    cluster dw 0

    ; Pre-compute segments to save space for now. This could
    ; be done by using the BPB values instead.
    FAT1_SEGMENT            equ 0x7C00 + 512 * 1
    FAT2_SEGMENT            equ FAT1_SEGMENT + 512 * 9 * 1
    ROOT_DIRECTORY_SEGMENT  equ FAT2_SEGMENT + 512 * 9 * 1
    DATA_SEGMENT            equ ROOT_DIRECTORY_SEGMENT + 512 * 14 * 1
    INIT_SEGMENT            equ 0x1000

    ; Strings.
    FILE_INIT_BIN           db "INIT    BIN"
    FILE_KERNEL_BIN         db "KERNEL  BIN"
    ERROR_FILE_NOT_FOUND    db "Could not find file.", 0
    ERROR_READ_FROM_DRIVE   db "Could not read from drive.", 0

; find_file
; Find a file in the root directory.
;
; Input:
; si = filename
;
; Output:
; di = entry

find_file:
    mov ax, [DIRECTORY_ENTRIES] ; File count.
    mov di, ROOT_DIRECTORY_SEGMENT

.loop:
    ; Store source and destination as they
    ; will be incremented by the comparison.
    push si
    push di

    ; Compare.
    mov cx, 11                  ; Filename is 11 bytes.
    repe cmpsb                  ; Compare filename.

    ; Restore source and destination.
    pop di
    pop si

    jz .end

    add di, 0x20                ; Next entry in directory.
    dec ax
    jnz .loop

.error:
    mov si, ERROR_FILE_NOT_FOUND
    jmp error

.end:
    ret

; read_file
; Read file from disk to memory.
;
; Input:
; es:bx = destination
; ax    = cluster

read_file:
    mov word [cluster], ax      ; Store cluster.

.loop:
    ; Compute LBA of cluster.
    mov si, word [cluster]
    sub si, 2                   ; Reserved.
    add si, 1+9+9+14            ; Data segment.

    mov al, [SECTORS_PER_CLUSTER] ; Number of sectors.
    mov dl, byte [boot_drive]   ; Drive.
    call read16

    ; Increment destination.
    mov ax, word [BYTES_PER_SECTOR]
    mov cx, word [SECTORS_PER_CLUSTER]
    mul cx
    add bx, ax

    ; Compute next cluster.
    mov ax, word [cluster]
    mov cx, ax 
    mov dx, ax
    shr dx, 1                   ; Multiply by 1.5 bytes.
    add cx, dx

    ; Read word from FAT.
    push bx                     ; Store destination.
    mov bx, FAT1_SEGMENT
    add bx, cx
    mov dx, word [bx]
    pop bx                      ; Restore destination.

    ; Check if cluster is even or odd to find
    ; where the 12 bits are.
    test ax, 1
    jz .low

.high:
    ; Take low bits bits.
    shr dx, 4
    jmp .next 
.low:
    ; Take high 12 bits.
    and dx, 0xFFF

.next:
    mov word [cluster], dx      ; Store next cluster.
    cmp dx, 0xFF0               ; Check for EOF.
    jb .loop

.end:
    retn

; read16
; Read sectors from disk to memory.
;
; Input:
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
    div word [SECTORS_PER_TRACK]
    mov cl, dl
    inc cl
    xor dx, dx
    div word [HEADS_PER_CYLINDER]
    mov dh, dl
    mov ch, al
    shl ah, 6 
    or cl, ah

    pop ax                      ; Restore drive.
    mov dl, al
    pop ax                      ; Restore number of sectors.

    int 0x13                    ; Read sectors.
    jnc .end

.error:
    mov si, ERROR_READ_FROM_DRIVE
    jmp error

.end:
    popa
    retn
    