#include <stddef.h>
#include "filesystem/elf.h"

bool elf_check_file(const elf_header *header)
{
    return header != NULL &&
           header->e_ident[EI_MAG0] == 0x7F &&
           header->e_ident[EI_MAG1] == 'E' &&
           header->e_ident[EI_MAG2] == 'L' &&
           header->e_ident[EI_MAG3] == 'F';
}

bool elf_check_support(const elf_header *header)
{
    return header != NULL &&
           header->e_ident[EI_CLASS] == 0x1 &&
           header->e_ident[EI_DATA] == 0x1 &&
           header->e_ident[EI_VERSION] == 0x1 &&
           header->e_machine == 0x3 &&
           header->e_type == ET_EXEC;
}