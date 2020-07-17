section .boot32
bits 32

global boot32
boot32:                      
    ; Set up segment registers.
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    ; Print name.
    mov esi, name
    call print32

    ; Get boot drive.
    mov [boot_drive], dl        ; TODO: Store in boot16.

    ; Get CPU vendor.
    mov eax, 0
    cpuid                       ; TODO: Check for cpuid capability.
    mov [cpuid_vendor1], ebx
    mov [cpuid_vendor2], edx
    mov [cpuid_vendor3], ecx

    ; Get CPU features.
    mov eax, 1
    cpuid
    mov [cpuid_features1], edx
    mov [cpuid_features2], ecx

    ; Set up stack.
    mov ebp, 0x90000
    mov esp, 0x90000

    ; Call kernel.
    push boot_info
    extern main
    call main
    cli
    hlt

; Boot information structure.
boot_info:
    ; GDT.
    extern gdt_desc
    gdt: dd gdt_desc

    ; Drive.
    boot_drive: db 0

    ; CPU.
    cpuid_vendor1: dd 0
    cpuid_vendor2: dd 0
    cpuid_vendor3: dd 0
    cpuid_vendor4: db 0
    cpuid_features1: dd 0
    cpuid_features2: dd 0

    ; Memory map.
    global mmap_count
    mmap_count: dd 0
    global mmap_entries
    mmap_entries: dd 0x8000
    
name: db "(Boot32)", 0

; print32
; Print a string on the screen.
;
; esi   =   address of string

print32:
    pusha

    mov ebx, 0xB8000

.loop:
    lodsb
    or al, al
    jz .end
    or eax, 0x3F00
    mov word [ebx], ax
    add ebx, 2
    jmp .loop

.end:
    popa
    retn