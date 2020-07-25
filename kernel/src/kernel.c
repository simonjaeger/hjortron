#include "kernel.h"
#include "types.h"
#include "string.h"

#include "memory/malloc.h"

#include "display/display.h"
#include "display/colors.h"
#include "cpu/irq.h"
#include "cpu/pic.h"

#include "drivers/keyboard.h"
#include "device/pci.h"
#include "device/serial.h"
#include "device/ata.h"
#include "debug.h"

#include "filesystem/fs.h"
#include "filesystem/fat12.h"

void disk_info(const boot_info *boot_info)
{
    string type;
    if (boot_info->drive > 0xE0)
    {
        type = "hdd";
    }
    else if (boot_info->drive == 0xE0)
    {
        type = "cd-rom";
    }
    else if (boot_info->drive > 0x7F)
    {
        type = "hdd";
    }
    else
    {
        type = "fdd";
    }
    debug("drive=%x type=%s", boot_info->drive, type);
}

void cpuid_info(const cpuid *cpuid)
{
    debug("vendor=%s features=%x", cpuid->vendor, cpuid->features1);
}

void mmap_info(const memory_map *memory_map)
{
    for (size_t i = 0; i < memory_map->count; i++)
    {
        memory_map_entry *entry = &memory_map->entries[i];
        string type = "unknown";
        if (entry->type == E820_ENTRY_FREE)
        {
            type = "free";
        }
        else if (entry->type == E820_ENTRY_RESERVED)
        {
            type = "reserved";
        }
        debug("base=%lx length=%lx type=%s", entry->base, entry->length, type);
    }
}

void main(const boot_info *boot_info)
{
    display_init();

    printf("%f(kernel)\n", (text_attribute){COLOR_CYAN, COLOR_WHITE});

    disk_info(boot_info);
    cpuid_info(&boot_info->cpuid);
    mmap_info(&boot_info->memory_map);

    // TODO: Find appropriate memory map entry for dynamic memory.
    malloc_init(0x30000, 1024 * 1024 * 16);

    irq_init();
    pic_init();

    irq_enable();

    keyboard_init();
    pci_init();

    serial_init(SERIAL_COM1);
    ata_init();

    fs_driver *fat12_driver = fat12_init((fat12_extended_bios_parameter_block *)(uint32_t)boot_info->bpb);
    fs_mount(fat12_driver, 'H');

    fs_file *file = fs_open("/H/DATA1      /DATA2      /LOREM   TXT");
    // fs_file *file = fs_open("/H/KERNEL  BIN");

    if (file == NULL)
    {
        printf("%s", "Cannot open file.\n");
    }
    else
    {
        printf("Opened file: \"%s\", %d bytes, %d ref \n", file->name, file->len, file->ref);

        size_t buffer_size = 1024;
        uint32_t *buffer = (uint32_t *)malloc(buffer_size + 1);

        // fs_seek(file, 0x880);
        while (file->offset < file->len)
        // for (size_t i = 0; i < 5 * 8; i++)
        {
            strset((string)buffer, '\0', buffer_size + 1);
            fs_read(file, buffer, buffer_size);
            printf("%s", buffer);
            // debug("%s", buffer);

            // debug("%x %x", file->offset, file->len);
        }

        fs_close(file);

        free(buffer);
    }

    while (1)
        ;
}
