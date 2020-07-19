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
    fats                                db 0
    directory_entries                   dw 0
    sectors                             dw 0
    media_descriptor_type               db 0
    sectors_per_fat                     dw 0
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

; Pre-computing segments to save space for now. This could
; be done by using the BPB values instead.
FAT1_SEGMENT equ 0x7C00 + 512 * 1
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
    mov dl, byte [boot_drive]   ; Drive.
    call read16

    ; Read root directory.
    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, ROOT_SEGMENT        ; Destination offset.
    mov si, 1+9+9               ; First sector.
    mov al, 14                  ; Number of sectors.
    mov dl, byte [boot_drive]   ; Drive.
    call read16



    mov si, FILE_TEST_BIN
    call find

    mov si, di
    call print

    mov ax, [di+0x1A]    ; Get cluster.

    mov bx, 0x3000
    call read

    jmp halt

error:
    call print

halt:
    jmp halt
    hlt

    boot_drive db 0
    cluster dw 0


    cluster_a dw 0
    test_file db "TEST    BIN"
    file_found db "Found file!", 0
    done db "done!", 0

    FILE_FOUND            db "File found", 0
    FILE_NOT_FOUND            db "File not found", 0

    FILE_TEST_BIN            db "TEST    BIN"
    FILE_KERNEL_BIN            db "TEST    BIN"


; find
; Find a file in the root directory.
;
; Input:
; si = filename
;
; Output:
; di = entry

find:
    mov ax, [directory_entries] ; File count.
    mov di, ROOT_SEGMENT

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
    mov si, FILE_NOT_FOUND
    jmp error

.end:
    ret

; read
; Read file from disk to memory.
;
; Input:
; es:bx = destination
; ax    = cluster

read:
    mov word [cluster], ax      ; Store cluster.

.loop:
    ; Compute LBA of cluster.
    mov si, word [cluster]
    sub si, 2                   ; Reserved.
    add si, 1+9+9+14            ; Data segment.

    mov al, [sectors_per_cluster] ; Number of sectors.
    mov dl, byte [boot_drive]   ; Drive.
    call read16

    ; Temp.
    mov si, bx
    call print

    ; Increment destination.
    mov ax, word [bytes_per_sector]
    mov cx, word [sectors_per_cluster]
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
    
; print
; Print string using teletype output.
;
; Input:
; si = string

print:
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