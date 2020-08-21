#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "kernel.h"
#include "string.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "display/colors.h"
#include "cpu/irq.h"
#include "cpu/pic.h"
#include "drivers/pit.h"
#include "drivers/keyboard.h"
#include "drivers/pci.h"
#include "drivers/serial.h"
#include "drivers/ata.h"
#include "drivers/cmos.h"
#include "debug.h"
#include "cpu/boot.h"
#include "cpu/exceptions.h"
#include "filesystem/fs.h"
#include "filesystem/fat12.h"
#include "assert.h"
#include "execution/syscall.h"
#include "execution/elf.h"
#include "execution/scheduler.h"
#include "list.h"

void main(const boot_info *boot_info)
{
    /* Display environment information. */
    display_init();
    printf("%f(kernel)\n", (text_attribute){COLOR_CYAN, COLOR_WHITE});

    disk_info(boot_info);
    cpuid_info(&boot_info->cpuid);
    mmap_info(&boot_info->memory_map);

    /* Initialize dynamic memory. */
    malloc_init(&boot_info->memory_map);

    /* Initialize IRQ's. */
    exceptions_init();
    irq_init();
    pic_init();
    irq_enable();
    syscall_init();
    scheduler_init();

    /* Initialize drivers. */
    pit_init();
    keyboard_init();
    pci_init();
    serial_init(SERIAL_COM1);
    ata_init();

    /* TODO: Skip if there is no disk on primary ATA bus. */
    fs_driver *fat12_driver = fat12_init((fat12_extended_bios_parameter_block *)(uint32_t)boot_info->bpb);
    fs_mount(fat12_driver, 'H');
    /* Test ELF load. */
    fs_file *file = fs_open("/H/APPS/TEST.ELF");
    if (file == NULL)
    {
        printf("Could not open TEST.ELF.");
    }
    else
    {
        /* Read file. */
        void *elf_buffer;
        uint32_t elf_entry;
        elf_read(file, &elf_buffer, &elf_entry);
        uint32_t (*elf_main)(void) = (uint32_t(*)(void))((uint32_t)elf_buffer + elf_entry);

        /* Close file. */
        fs_close(file);

        /* Run file. */
        process_t *process = process_create((uint32_t *)elf_main);
        scheduler_start(process);
        // elf_main();
    }

    /* Enable scheduler. */
    scheduler_enable();

    // /* Test readdir. */
    // fs_dir *dir = fs_opendir("/H/APPS");
    // if (dir == NULL)
    // {
    //     printf("Could not open /H/APPS");
    // }
    // else
    // {
    //     printf("Opened /H/APPS %s %d\n", dir->name, dir->ref);

    //     fs_dirent *dirents;
    //     size_t dirent_len;
    //     fs_readdir(dir, &dirents, &dirent_len);

    //     for (size_t i = 0; i < dirent_len; i++)
    //     {
    //         debug("%s %d", dirents[i].name, dirents[i].type);
    //     }

    //     fs_closedir(dir);
    // }

    // /* Test syscall. */
    // asm volatile("int $0x80" ::"a"(SYSCALL_SLEEP), "b"(500));
    // asm volatile("int $0x80" ::"a"(SYSCALL_STOP));
    // uint32_t test;
    // asm volatile("int $0x80" ::"a"(1), "b"(&test));
    // debug("%x", test);

    while (1)
        ;
}