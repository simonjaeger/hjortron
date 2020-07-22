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

typedef struct fat12_directory_entry {
    char filename[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t created_time_s;
    uint16_t created_time;
    uint16_t created_date;
    uint16_t accessed;
    uint16_t cluster_high;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t cluster_low;
    uint32_t size;
} fat12_directory_entry;

#endif // FS_FAT12_H_