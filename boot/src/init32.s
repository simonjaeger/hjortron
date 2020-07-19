section .text
bits 32

global init32
init32:
    ; Set up segment registers.
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
    
    ; Get boot drive.
    mov dx, 0x0                 ; TODO. Fix...
    mov [boot_drive], dl        ; TODO: Store in init16.

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

    ; Print name.
    mov esi, name
    extern print32
    call print32

    ; ; Call kernel.
    push data
    call 0x10000
    cli

    jmp halt

error:
    extern print32
    call print32

halt:
    jmp halt
    hlt

data:
    ; GDT.
    extern GDT
    gdt: dd GDT

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