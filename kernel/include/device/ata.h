#ifndef ATA_H_
#define ATA_H_

#include "types.h"
#include "string.h"

typedef struct ata_identify_device_data
{
    struct
    {
        uint8_t reserved1 : 1;
        uint8_t retired1 : 1;
        uint8_t response_incomplete : 1;
        uint8_t retired2 : 3;
        uint8_t obsolete1 : 1;
        uint8_t retired3 : 7;
        uint8_t zero1 : 1;
    };
    uint16_t obsolete2;
    uint16_t specific_configuration;
    uint16_t obsolete3;
    uint16_t retired4[2];
    uint16_t reserved2[2];
    uint16_t retired5;
    uint16_t obsolete4;
    uint8_t serial_number[20];
    uint16_t retired6[2];
    uint16_t obsolete5;
    uint8_t firmware_revision[8];
    uint8_t model_number[40];
    // TODO: ...
} ata_identify_device_data;

#define ATA_BUS_PRIMARY 0x1F0
#define ATA_BUS_SECONDARY 0x170

void ata_init();
void ata_string_swap(string buffer, uint8_t *data, size_t length);

void ata_read(uint16_t *buffer, uint16_t bus, uint32_t lba, uint32_t sector_count);

#endif // ATA_H_