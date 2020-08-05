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
           header->e_type == ET_REL;
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
    }

    for (size_t i = 0; i < header->e_shnum; i++)
    {
        if (section_headers[i].sh_type != SHT_REL)
        {
            continue;
        }

        // Go through each symbol to relocate.
        elf_rel *rels = (elf_rel *)&file_buffer[section_headers[i].sh_offset];
        elf_section_header *sym_section_header = &section_headers[section_headers[i].sh_link];
        elf_section_header *target_section_header = &section_headers[section_headers[i].sh_info];

        for (size_t j = 0; j < section_headers[i].sh_size / section_headers[i].sh_entsize; j++)
        {
            uint8_t type = rels[j].info;
            uint32_t sym_offset = rels[j].info >> 8;
            if (!type || !sym_offset)
            {
                continue;
            }

            // TODO: Check if symbol index is within range.

            __attribute__((unused)) elf_sym *sym = &((elf_sym *)&file_buffer[sym_section_header->sh_offset])[sym_offset];

            debug("%d %x %x %x", j, rels[j].offset, rels[j].info, sym->value);

            __attribute__((unused)) uint32_t *ref = (uint32_t *)&mem_buffer[target_section_header->sh_addr + rels[j].offset];

            // debug("%d %d %x %x", ((uint32_t)type), ((uint32_t)sym), sym->value, ref);
            __attribute__((unused)) uint32_t symval = 0;

            if (sym->shndx == SHN_UNDEF)
            {
                debug("%s", "cannot find symbol");
                return NULL;
            }

            if (sym->shndx == SHN_ABS)
            {
                symval = sym->value;
            }
            else
            {
                elf_section_header *temp = &section_headers[sym->shndx];
                symval = (uint32_t)mem_buffer + sym->value + temp->sh_addr;
                // offset = file_buffer[sym_section_header->sh_offset]
            }

            // offset += (uint32_t)mem_buffer;

            switch (type)
            {
            case RELT_386_NONE:
                break;
            case RELT_386_32:
                *ref += symval;
                // *ref += sym->value + (uint32_t)mem_buffer;
                break;
            case RELT_386_PC32:
                *ref += symval - (uint32_t)ref;
                // *ref += sym->value + (uint32_t)mem_buffer - (uint32_t)ref;
                break;
            default:
                debug("%s", "cannot relocate symbol");
                break;
            }
        }
    }

    free(file_buffer);
    return mem_buffer;
}