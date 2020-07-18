section .boot16
bits 16

global boot16
boot16:
    ; BIOS parameter block.
    jmp short start
    nop

    OemIdentifier                   dq 0
    BytesPerSector                  dw 0
    SectorsPerCluster               db 0
    ReservedSectors                 dw 0
    FileAllocationTables            db 0
    DirectoryEntries                dw 0
    TotalSectors                    dw 0
    MediaDescriptorType             db 0
    SectorsPerFileAllocationTable   dw 0
    SectorsPerTrack                 dw 0
    HeadsPerCylinder                dw 0
    HiddenSectors                   dd 0
    LargeSectors                    dd 0

    ; Extended boot record (FAT12).
    DriveNumber                     db 0
    Reserved                        db 0
    Signature                       db 0
    VolumeId                        dd 0
    VolumeLabel                     times 11 db 0
    SystemIdentifier                dq 0

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




    mov cx, [DirectoryEntries]
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


    File db "TEST1   TXT"


    ; Set up stack < 0x7C00.
    xor ax, ax              
    cli                         ; Should be disabled.
    mov ss, ax
    mov sp, 0x7C00

    ; Enable A20 address line.
    int 0x15                

    ; Set VGA text mode 3.
    mov ax, 0x3                 ; Set VGA text mode 3.
    int 0x10            

    ; Disable cursor.
    mov ah, 0x01           
    mov ch, 0x3F
    int 0x10

    ; ; Check sector size to determine read method.
    ; extern sector_size
    ; mov ax, sector_size
    ; cmp ax, 2048
    ; je read2048

; read512:
;     ; Read boot32.
;     xor ax, ax
;     mov es, ax                  ; Destination segment.
;     mov bx, boot32              ; Destination offset.
;     mov al, 1                   ; Number of sectors.
;     mov ch, 0                   ; Cylinder.
;     mov dh, 0                   ; Head.
;     mov cl, 2                   ; Start sector.
;     ; mov dl, dl                ; Drive "should" be set by BIOS.
;     extern boot32
;     call read16

;     ; Read kernel.
;     mov ax, 0x1000
;     mov es, ax                  ; Destination offset.
;     mov bx, 0                   ; Destination segment.
;     extern kernel_size          ; Defined in linker.
;     mov al, kernel_size         ; Number of sectors.
;     mov ch, 0                   ; Cylinder 0.
;     mov dh, 0                   ; Head 0.
;     mov cl, 3                   ; Start sector.
;     ; mov dl, dl                ; Drive "should" be set by BIOS.
;     call read16

    jmp read_end

; read2048:
;     ; Read kernel.
;     extern kernel_size                  ; Defined in linker.
;     mov word [dap_sectors], kernel_size ; Number of sectors.
;     mov word [dap_segment], 0           ; Destination.
;     mov word [dap_offset], 0x1000       ; Destination.
;     mov ebx, [bi_file_location]         ; Get sector after boot to find kernel.
;     inc ebx                     
;     mov [dap_start], ebx       
;     ; mov dl, dl                        ; Drive "should" be set by BIOS.
;     call read16_ext

read_end:
    ; ; Read memory map.
    ; call read_e820

    ; Load GDT.
    cli                         
    lgdt [gdt_desc]             

    ; Enable protected mode.
    mov eax, cr0                ; Set lowest bit of cr0.
	or eax, 0x1
	mov cr0, eax

    ; ; Jump to boot32.
    ; extern boot32
	; jmp 0x8:boot32              ; Far jump to load CS register.

; Set up GDT.
gdt_null:
    dq 0x0                      ; Null segment.
gdt_code:                       ; Code segment.
    dw 0xFFFF, 0
    db 0, 0x9A, 0xCF, 0
gdt_data:                       ; Data segment.
    dw 0xFFFF, 0
    db 0, 0x92, 0xCF, 0
global gdt_desc
gdt_desc:                       ; Table descriptor (size and offset).
    dw gdt_desc - gdt_null - 1
    dd gdt_null

; read_e820
; Detect memory map using E820 function.

; SMAP equ 0x0534D4150

; read_e820:
;     pusha

;     ; Reset registers.
;     xor ebx, ebx
;     xor bp, bp                  ; Counter.

;     ; Set destination.
;     xor ax, ax
;     mov es, ax
;     extern mmap_entries
;     mov di, [mmap_entries]

; .loop:
;     mov edx, SMAP               ; Set to "SMAP".
;     mov eax, 0xE820             ; Set function.

;     mov ecx, 24                 ; Read 24 bytes.
;     int 0x15
;     jc short .fail              ; Check for error (unsupported).

;     mov edx, SMAP               ; Set to "SMAP" again.
;     cmp eax, edx                ; Expect "SMAP" on success.
;     jne short .fail

;     inc bp                      ; Increment counter.

;     cmp ebx, 0                  ; Check for no more data.
;     je .end

;     ; TODO: Skip "empty" entries (length = 0).

;     add di, 24                  ; Increment destination.
;     jmp .loop

; .fail:
;     ; TODO: Handle error.

; .end:
;     extern mmap_count
;     mov [mmap_count], bp        ; Save counter.
;     clc                         ; Clear carry.

;     popa
;     retn




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
    ; C = (LBA / SectorsPerTrack) / HeadsPerCylinder
    ; H = (LBA / SectorsPerTrack) % HeadsPerCylinder
    ; S = (LBA % SectorsPerTrack) + 1
    xor dx, dx
    div word [SectorsPerTrack]
    mov cl, dl  
    inc cl
    xor dx, dx
    div word [HeadsPerCylinder]
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

; ; read16_ext
; ; Read sectors from disk to memory. Extended function.
; ;
; ; dap   =   (see below)
; ; dl    =   drive

; dap:
; dap_size:                   db 0x10 ; Size.
; dap_zero:                   db 0    ; Zero area.
; dap_sectors:                dw 0    ; Number of sectors.
; dap_segment:                dw 0    ; Destination.
; dap_offset:                 dw 0    ; Destination
; dap_start:                  dq 0    ; Sector start.

; read16_ext:
;     pusha

;     xor ax, ax          
;     mov ds, ax                  ; Disk address packet segment.
;     mov si, dap                 ; Disk address packet offset.

;     mov ah, 0x42                ; Read sectors.
;     int 0x13

;     ; TODO: Check status and handle error.

;     popa
;     retn

text: db "(boot16): !", 0
