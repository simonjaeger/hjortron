#include "fs/fs.h"
#include "fs/fat12.h"
#include "display/display.h"
#include "memory/malloc.h"
#include "device/ata.h"
#include "debug.h"

static fat12_extended_bios_parameter_block *bios_parameter_block = NULL;

void fs_init(const boot_info *boot_info)
{
    bios_parameter_block = (fat12_extended_bios_parameter_block *)(uint32_t)boot_info->bpb;

    char oem_identifier[9];
    strset(oem_identifier, '\0', 9);
    strcpy(bios_parameter_block->oem_identifier, oem_identifier, 8);

    char volume_label[12];
    strset(volume_label, '\0', 12);
    strcpy(bios_parameter_block->volume_label, volume_label, 11);

    char system_identifier[9];
    strset(system_identifier, '\0', 9);
    strcpy(bios_parameter_block->system_identifier, system_identifier, 8);

    printf("FAT12 Information:\n");
    printf("OEM Identifier: %s\n", oem_identifier);
    printf("System Identifier: %s\n", system_identifier);
    printf("Volume Identifier: %d\n", bios_parameter_block->volume_identifier);
    printf("Volume Label: %s\n", volume_label);
    printf("Total sectors: %d\n", bios_parameter_block->sectors);
    printf("Root entries: %d\n", bios_parameter_block->directory_entries);
    printf("Sectors per FAT: %d\n", bios_parameter_block->sectors_per_fat);
    printf("Sectors per cluster: %d\n", bios_parameter_block->sectors_per_cluster);
    printf("Sectors per track: %d\n", bios_parameter_block->sectors_per_track);
    printf("Bytes per sector: %d\n", bios_parameter_block->bytes_per_sector);
    printf("Reserved sectors: %d\n", bios_parameter_block->reserved_sectors);

    fs_ls_test();
}

void fs_ls_test()
{
    uint16_t *buffer = (uint16_t *)malloc(256 * sizeof(uint16_t));

    size_t lba = 0;
    lba += bios_parameter_block->reserved_sectors;
    for (size_t i = 0; i < bios_parameter_block->fats; i++)
    {
        lba += bios_parameter_block->sectors_per_fat;
    }

    ata_read(buffer, ATA_BUS_PRIMARY, lba, 1);

    printf("\ntest ls /\n%s    %s   %s\n", "Filename", "Type", "Size");
    fat12_directory_entry *entries = (fat12_directory_entry *)buffer;
    for (size_t i = 0; i < bios_parameter_block->directory_entries; i++)
    {
        fat12_directory_entry entry = entries[i];
        if (entry.attributes == 0x10 || entry.attributes == 0x20)
        {
            char filename[12];
            strset(filename, '\0', 12);
            strcpy(entry.filename, filename, 11);
            string type = entry.attributes == 0x10 ? "FOLDER" : "FILE  ";
            printf("%s %s %d\n", filename, type, entry.size);
        }
    }

    free(buffer);
}