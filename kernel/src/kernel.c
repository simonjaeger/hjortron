#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "string.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "display/colors.h"
#include "cpu/irq.h"
#include "cpu/pic.h"
#include "drivers/keyboard.h"
#include "drivers/pci.h"
#include "drivers/serial.h"
#include "drivers/ata.h"
#include "debug.h"
#include "boot.h"
#include "exceptions.h"
#include "filesystem/fs.h"
#include "filesystem/fat12.h"
#include "filesystem/elf.h"

void main(const boot_info *boot_info)
{
    // Display environment information.
    display_init();
    printf("%f(kernel)\n", (text_attribute){COLOR_CYAN, COLOR_WHITE});

    disk_info(boot_info);
    cpuid_info(&boot_info->cpuid);
    mmap_info(&boot_info->memory_map);

    malloc_init(&boot_info->memory_map);

    // Initialize IRQ's.
    exceptions_init();
    irq_init();
    pic_init();
    irq_enable();

    // Initialize drivers.
    keyboard_init();
    pci_init();
    serial_init(SERIAL_COM1);
    ata_init();

    // TODO: Skip if there is no disk on primary ATA bus.
    fs_driver *fat12_driver = fat12_init((fat12_extended_bios_parameter_block *)(uint32_t)boot_info->bpb);
    fs_mount(fat12_driver, 'H');

    // Test ELF load.
    fs_file *file = fs_open("/H/TEST.ELF");
    if (file == NULL)
    {
        printf("Could not open TEST.ELF.");
    }
    else
    {
        // Read file.
        void *elf_buffer;
        uint32_t elf_entry;
        elf_read(file, &elf_buffer, &elf_entry);
        __attribute__((unused)) uint32_t (*elf_main)(void) = (uint32_t(*)(void))((uint32_t)elf_buffer + elf_entry);

        // Close file.
        fs_close(file);

        // // Run file.
        // elf_main();
    }

    while (1)
        ;
}