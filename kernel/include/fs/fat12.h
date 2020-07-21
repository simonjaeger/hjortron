#ifndef FS_FAT12_H_
#define FS_FAT12_H_

#include "types.h"

typedef struct fat12_extended_bios_parameter_block
{
    char jmp[3];
    char oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fats;
    uint16_t directory_entries;
    uint16_t sectors;
    uint8_t media_descriptor_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads_per_cylinder;
    uint32_t hidden_sectors;
    uint32_t large_sectors;

    // Extended boot record (FAT12).
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t signature;
    uint32_t volume_identifier;
    char volume_label[11];
    char system_identifier[8];
} __attribute__((packed)) fat12_extended_bios_parameter_block;

#endif // FS_FAT12_H_