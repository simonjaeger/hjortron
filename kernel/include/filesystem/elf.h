#ifndef FILESYSTEM_ELF_H_
#define FILESYSTEM_ELF_H_

#include <stdint.h>
#include <stdbool.h>
#include "filesystem/fs.h"

#define EI_MAG0 0x0
#define EI_MAG1 0x1
#define EI_MAG2 0x2
#define EI_MAG3 0x3
#define EI_CLASS 0x4
#define EI_DATA 0x5
#define EI_VERSION 0x6
#define EI_OSABI 0x7
#define EI_ABIVERSION 0x8
#define EI_PAD 0x9

#define ET_NONE 0x0
#define ET_REL 0x1
#define ET_EXEC 0x2
#define ET_DYN 0x3
#define ET_CORE 0x4
#define ET_LOOS 0xFE00
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

typedef struct elf_header
{
    uint8_t e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} elf_header;

typedef struct elf_program_header
{
    uint32_t ph_type;
    uint32_t ph_offset;
    uint32_t ph_vaddr;
    uint32_t ph_paddr;
    uint32_t ph_filesz;
    uint32_t ph_memsz;
    uint32_t ph_flags;
    uint32_t ph_align;
} elf_program_header;

typedef struct elf_section_header
{
    uint32_t sh_name;
    uint32_t sh_type;
    uint32_t sh_flags;
    uint32_t sh_addr;
    uint32_t sh_offset;
    uint32_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint32_t sh_addralign;
    uint32_t sh_entsize;
} elf_section_header;

bool elf_check_file(const elf_header *header);
bool elf_check_support(const elf_header *header);
void *elf_read(fs_file *file);

#endif // FILESYSTEM_ELF_H_