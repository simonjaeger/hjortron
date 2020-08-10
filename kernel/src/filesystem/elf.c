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

elf_program_header *elf_find_ph(const elf_header *header, const size_t idx)
{
    return (elf_program_header *)((uint32_t)header + header->e_phoff + sizeof(elf_program_header) * idx);
}

elf_section_header *elf_find_sh(const elf_header *header, const size_t idx)
{
    return (elf_section_header *)((uint32_t)header + header->e_shoff + sizeof(elf_section_header) * idx);
}

elf_sym *elf_find_sym(const elf_header *header, size_t sh_idx, size_t sym_idx)
{
    elf_section_header *sh = elf_find_sh(header, sh_idx);
    elf_sym *sym = (elf_sym *)((uint32_t)header + sh->sh_offset + sym_idx * sizeof(elf_sym));

    if (sym->st_shndx == SHN_UNDEF)
    {
        return NULL;
    }
    return sym;
}

bool elf_rel_sym(uint8_t type, uint32_t *target, uint32_t value)
{
    switch (type)
    {
    case RELT_386_NONE:
        break;
    case RELT_386_32:
        *target += value;
        break;
    case RELT_386_PC32:
        *target += value - (uint32_t)target;
        break;
    default:
        return false;
    }
    return true;
}

void elf_read(fs_file *file, void **buffer, uint32_t *entry)
{
    // Read file.
    uint8_t *file_buffer = (uint8_t *)malloc(file->len);
    fs_read(file, (uint32_t *)file_buffer, file->len);

    // Check file and support.
    const elf_header *header = (elf_header *)file_buffer;
    if (!elf_check_file(header) || !elf_check_support(header))
    {
        error("%s", "cannot read file");
        free(file_buffer);
        return;
    }

    // Compute length of buffer.
    size_t len = 0;
    for (size_t i = 0; i < header->e_phnum; i++)
    {
        const elf_program_header *ph = elf_find_ph(header, i);
        if (ph->ph_type == PHT_LOAD)
        {
            continue;
        }
        len += ph->ph_memsz;
    }

    uint8_t *reloc_buffer = (uint8_t *)malloc(len);
    memset(reloc_buffer, 0, len);

    // Copy PROGBITS to buffer.
    for (size_t i = 0; i < header->e_shnum; i++)
    {
        const elf_section_header *sh = elf_find_sh(header, i);
        if (!(sh->sh_flags & SHF_ALLOC))
        {
            continue;
        }

        // Skip sections like .bss.
        if (sh->sh_type != SHT_PROGBITS)
        {
            continue;
        }

        memcpy(&reloc_buffer[sh->sh_addr], &file_buffer[sh->sh_offset], sh->sh_size);
    }

    // Go through each symbol to relocate.
    for (size_t i = 0; i < header->e_shnum; i++)
    {
        const elf_section_header *sh = elf_find_sh(header, i);
        const elf_section_header *info_sh = elf_find_sh(header, sh->sh_info);

        if (sh->sh_type != SHT_REL)
        {
            continue;
        }

        const elf_rel *rels = (elf_rel *)&file_buffer[sh->sh_offset];
        for (size_t j = 0; j < sh->sh_size / sh->sh_entsize; j++)
        {
            const uint8_t type = rels[j].r_info;
            const uint32_t offset = rels[j].r_info >> 8;
            if (!type || !offset)
            {
                continue;
            }

            const elf_sym *sym = elf_find_sym(header, sh->sh_link, offset);
            if (sym == NULL)
            {
                error("%s", "cannot find symbol");
                free(file_buffer);
                return;
            }

            // Compute symbol value.
            uint32_t value = 0;
            if (sym->st_shndx == SHN_ABS)
            {
                value = sym->st_value;
            }
            else
            {
                value = (uint32_t)reloc_buffer + sym->st_value + elf_find_sh(header, sym->st_shndx)->sh_addr;
            }

            // Relocate symbol.
            uint32_t *target = (uint32_t *)&reloc_buffer[info_sh->sh_addr + rels[j].r_offset];
            if (!elf_rel_sym(type, target, value))
            {
                error("%s", "cannot relocate symbol");
                free(file_buffer);
                return;
            }
        }
    }

    free(file_buffer);
    *buffer = reloc_buffer;
    *entry = header->e_entry;
}