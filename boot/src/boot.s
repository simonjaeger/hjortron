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

    mov byte [boot_drive], dl

    ; Read drive parameters if the boot drive was a HDD. 
    ; Otherwise, trust the BPB.
    mov al, dl
    and al, 0x80
    jz .load

    mov ah, 0x8
    int 0x13
    inc dh
    mov byte [HEADS_PER_CYLINDER], dh
    mov byte [SECTORS_PER_TRACK], cl

.load:
    ; Find INIT.BIN.
    mov si, FILE_INIT_BIN
    call find_file

    ; Load INIT.BIN.
    xor bx, bx
    mov es, bx
    mov bx, INIT_SEGMENT
    call read_file

    ; Find KERNEL.BIN.
    mov si, FILE_KERNEL_BIN
    call find_file

    ; Load KERNEL.BIN.
    mov bx, 0x1000
    mov es, bx
    xor bx, bx
    call read_file

    ; Jump to INIT.BIN.
    push OEM_IDENTIFIER
    push boot_drive
    jmp INIT_SEGMENT

error:
    extern print16
    push si
    mov si, ERROR_PREFIX
    call print16
    pop si
    call print16

halt:
    jmp halt
    hlt

data:
    ; Runtime data.
    boot_drive  db 0
    cluster     dw 0

    ; Runtime constants.
    FILE_INIT_BIN           db "INIT    BIN"
    FILE_KERNEL_BIN         db "KERNEL  BIN"
    ERROR_PREFIX            db "Cannot ", 0
    ERROR_FILE_NOT_FOUND    db "find file.", 0
    ERROR_READ_FROM_DRIVE   db "read drive.", 0

    ; Compilation constants.
    INIT_SEGMENT    equ 0x1000
    BUFFER          equ 0x7E00
    BUFFER_MAX      equ BUFFER + 0x200
    FAT_ENTRY_SIZE  equ 0x20

; find_file
; Find a file in the root directory.
;
; Input:
; si = filename
;
; Output:
; ax = cluster
; di = entry

find_file:
    mov ax, [DIRECTORY_ENTRIES] ; File count.
    mov di, BUFFER_MAX          ; Force a read of first sector.
    xor bx, bx                  ; Sector offset.

.loop:
    ; Read sector from root directory if we are
    ; beyond the buffer.
    cmp di, BUFFER_MAX
    jb .compare

    pusha                       ; Store registers.

    ; Compute LBA for root directory sector.
    ; Assuming 2 FAT's.
    mov si, word [RESERVED_SECTORS]
    add si, word [SECTORS_PER_FAT]
    add si, word [SECTORS_PER_FAT]
    add si, bx                  ; Add sector offset.

    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, BUFFER              ; Destination offset.
    mov al, 1                   ; Number of sectors.
    call read16

    popa                        ; Restore registers.

    mov di, BUFFER              ; Read from start of buffer.
    inc bx                      ; Increment sector offset.

.compare:
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

    add di, FAT_ENTRY_SIZE      ; Next entry in directory.

    dec ax
    jnz .loop

.error:
    mov si, ERROR_FILE_NOT_FOUND
    jmp error

.end:
    mov ax, [di+0x1A]           ; Set cluster.
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
    ; Load cluster, subtracted by reserved cluster.
    mov ax, word [cluster]
    sub ax, 2

    ; Multiply cluster and sectors per cluster.
    xor ch, ch
    mov cl, byte [SECTORS_PER_CLUSTER]
    mul cx

    ; Add offset of reserved sectors and FAT's.
    add ax, word [RESERVED_SECTORS] 
    add ax, word [SECTORS_PER_FAT]
    add ax, word [SECTORS_PER_FAT]
    mov si, ax

    ; Compute root clusters by multiplying root entries
    ; with the length of an entry and divided by bytes per sector. 
    xor dx, dx
    mov ax, word [DIRECTORY_ENTRIES]
    mov cx, FAT_ENTRY_SIZE
    mul cx
    mov cx, word [BYTES_PER_SECTOR]
    div cx
    add si, ax

    ; Read cluster sectors.
    mov al, byte [SECTORS_PER_CLUSTER]
    call read16

    ; Increment destination.
    mov ax, word [BYTES_PER_SECTOR]
    xor cx, cx
    mov cl, byte [SECTORS_PER_CLUSTER]
    mul cx
    add bx, ax

    ; Compute next cluster address in FAT by
    ; multiplying current cluster with 1.5.
    mov ax, word [cluster]
    mov cx, ax 
    mov dx, ax
    shr dx, 1
    add cx, dx

    ; Store next cluster address and destination.
    push ax
    push bx
    push es

    ; Divide next cluster address to determine
    ; sector (quotient) and offset (remainder).
    xor dx, dx
    mov ax, cx
    mov bx, word [BYTES_PER_SECTOR]
    div bx

    ; Compute sector within FAT1.
    mov si, word [RESERVED_SECTORS] ; FAT1
    add si, ax

    ; Load sector into buffer.
    xor ax, ax
    mov es, ax                  ; Destination segment.
    mov bx, BUFFER              ; Destination offset.
    mov al, 1                   ; Number of sectors.
    call read16

    ; Read next cluster from buffer.
    add dx, BUFFER
    mov bx, dx
    mov dx, word [bx]

    ; Restore registers.
    pop es
    pop bx
    pop ax

    ; Check if cluster is even or odd to find
    ; the 12 bits.
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
    ; Store next cluster and check if its the
    ; final cluster for this file.
    mov word [cluster], dx
    cmp dx, 0xFF0
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

read16:
    ; Store registers.
    pusha
    push ax

    ; Load LBA.
    mov ax, si

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

    ; Read sector.
    pop ax                      ; Restore number of sectors.
    mov ah, 0x02                ; Read sectors.
    mov dl, byte [boot_drive]   ; Drive.
    int 0x13                    ; Read sectors.
    jnc .end

.error:
    popa
    mov si, ERROR_READ_FROM_DRIVE
    jmp error

.end:
    popa
    retn
    