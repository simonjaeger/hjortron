#include <stddef.h>
#include "filesystem/elf.h"
#include "memory/malloc.h"
#include "string.h"
#include "debug.h"

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

void *elf_read(fs_file *file)
{
    // Read file.
    uint8_t *file_buffer = (uint8_t *)malloc(file->len);
    fs_read(file, (uint32_t *)file_buffer, file->len);

    // Check file and support.
    elf_header *header = (elf_header *)file_buffer;
    if (!elf_check_file(header) || !elf_check_support(header))
    {
        debug("%s", "cannot read file");
        free(file_buffer);
        return NULL;
    }

    elf_program_header *program_headers = (elf_program_header *)(&file_buffer[header->e_phoff]);
    elf_section_header *section_headers = (elf_section_header *)(&file_buffer[header->e_shoff]);

    // Compute length of memory buffer.
    size_t len = 0;
    for (size_t i = 0; i < header->e_phnum; i++)
    {
        if (program_headers[i].ph_type == PHT_LOAD)
        {
            continue;
        }
        len += program_headers[i].ph_memsz;
    }

    // Allocate and zero buffer.
    uint8_t *mem_buffer = (uint8_t *)malloc(len);
    memset(mem_buffer, 0, len);

    for (size_t i = 0; i < header->e_shnum; i++)
    {
        if (!(section_headers[i].sh_flags & SHF_ALLOC))
        {
            continue;
        }

        // Skip sections like .bss.
        if (section_headers[i].sh_type != SHT_PROGBITS)
        {
            continue;
        }

        memcpy(&mem_buffer[section_headers[i].sh_addr], &file_buffer[section_headers[i].sh_offset], section_headers[i].sh_size);

        // TODO: Relocation (symbols).
    }

    free(file_buffer);
    return mem_buffer;
}