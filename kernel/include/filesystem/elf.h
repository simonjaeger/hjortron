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

#define SHT_NULL 0x0	
#define SHT_PROGBITS 0x1
#define SHT_SYMTAB 0x2
#define SHT_STRTAB 0x3
#define SHT_RELA 0x4
#define SHT_HASH 0x5
#define SHT_DYNAMIC 0x6
#define SHT_NOTE 0x7
#define SHT_NOBITS 0x8
#define SHT_REL 0x9
#define SHT_SHLIB 0xA
#define SHT_DYNSYM_NULL 0xB
#define SHT_INIT_ARRAY 0xE
#define SHT_FINI_ARRAY 0xF
#define SHT_PREINIT_ARRAY 0x10
#define SHT_GROUP 0x11
#define SHT_SYMTAB_SHNDX 0x12
#define SHT_NUM	 0x13
#define SHT_LOOS 0x60000000

#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4
#define SHF_MERGE 0x10
#define SHF_STRINGS 0x20
#define SHF_INFO_LINK 0x40
#define SHF_LINK_ORDER 0x80
#define SHF_OS_NONCONFORMING 0x100
#define SHF_GROUP 0x200
#define SHF_TLS 0x400
#define SHF_MASKOS 0x0FF00000
#define SHF_MASKPROC 0xF0000000
#define SHF_ORDERED 0x4000000
#define SHF_EXCLUDE 0x8000000


#define PHT_NULL 0x0
#define PHT_LOAD 0x1
#define PHT_DYNAMIC 0x2
#define PHT_INTERP 0x3
#define PHT_NOTE 0x4
#define PHT_SHLIB 0x5
#define PHT_PHDR 0x6
#define PHT_TLS 0x7
#define PHT_LOOS 0x60000000
#define PHT_HIOS 0x6FFFFFFF
#define PHT_LOPROC 0x70000000
#define PHT_HIPROC 0x7FFFFFFF

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