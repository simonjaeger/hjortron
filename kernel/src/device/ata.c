#include "device/ata.h"
#include "cpu/io.h"
#include "display/display.h"
#include "debug.h"
#include "memory/malloc.h"

// #define ATA_BUS_PRIMARY 0x1F0
// #define ATA_BUS_SECONDARY 0x170

#define ATA_DISK_PRIMARY 0xA0
#define ATA_DISK_SECONDARY 0xB0

#define ATA_COMMAND_IDENTIFY 0xEC
#define ATA_COMMAND_READ 0x20

#define ATA_OFFSET_ERROR 0x1
#define ATA_OFFSET_FEATURES 0x1
#define ATA_OFFSET_SECTOR_COUNT 0x2
#define ATA_OFFSET_LBA0 0x3
#define ATA_OFFSET_LBA1 0x4
#define ATA_OFFSET_LBA2 0x5
#define ATA_OFFSET_DRIVE 0x6
#define ATA_OFFSET_COMMAND 0x7
#define ATA_OFFSET_STATUS 0x7
#define ATA_OFFSET_ALTSTATUS 0xC

void ata_wait(uint16_t bus)
{
    for (size_t i = 0; i < 4; i++)
    {
        inb(bus + ATA_OFFSET_ALTSTATUS);
    }
}

void ata_poll(uint16_t bus)
{
    uint8_t status;
    // Wait until the BSY bit is cleared.
    while ((status = inb(bus + ATA_OFFSET_STATUS)) & 0x80)
    {
    }

    // Wait until the DRQ bit is set.
    // TODO: Check for ERR bit.
    while (!((status = inb(bus + ATA_OFFSET_STATUS)) & 0x8))
    {
    }
}

void ata_identify(uint16_t bus, size_t disk)
{
    // Select disk.
    outb(bus + ATA_OFFSET_DRIVE, disk);

    // Reset sector count and LBA.
    outb(bus + ATA_OFFSET_SECTOR_COUNT, 0);
    outb(bus + ATA_OFFSET_LBA0, 0);
    outb(bus + ATA_OFFSET_LBA1, 0);
    outb(bus + ATA_OFFSET_LBA2, 0);

    // Send identify command.
    outb(bus + ATA_OFFSET_COMMAND, 0xEC);

    // Check for drive.
    uint8_t status = inb(bus + ATA_OFFSET_STATUS);
    if (!status)
    {
        debug("no drive, bus=%s, drive=%s",
              bus == ATA_BUS_PRIMARY ? "primary" : "secondary",
              disk == ATA_DISK_PRIMARY ? "primary" : "secondary");
        return;
    }

    ata_poll(bus);

    // TODO: Check LBA ports for non-zero (not ATA).

    // Check for ERR bit.
    if (status & 0x01)
    {
        debug("drive error, bus=%s, drive=%s",
              bus == ATA_BUS_PRIMARY ? "primary" : "secondary",
              disk == ATA_DISK_PRIMARY ? "primary" : "secondary");
        return;
    }

    // Read data for disk.
    uint16_t *data = (uint16_t *)malloc(sizeof(uint16_t) * 256);
    for (size_t i = 0; i < 256; i++)
    {
        data[i] = inw(bus);
    }

    ata_identify_device_data *device = (ata_identify_device_data *)data;

    char serial_number[21];
    char firmware_revision[9];
    char model_number[41];
    ata_string_swap(serial_number, device->serial_number, 20);
    ata_string_swap(firmware_revision, device->firmware_revision, 8);
    ata_string_swap(model_number, device->model_number, 40);
    strtrim(serial_number, ' ');
    strtrim(firmware_revision, ' ');
    strtrim(model_number, ' ');

    debug("detected drive, bus=%s, drive=%s, serial_number=%s, firmware_revision=%s, model_number=%s",
          bus == ATA_BUS_PRIMARY ? "primary" : "secondary",
          disk == ATA_DISK_PRIMARY ? "primary" : "secondary",
          serial_number,
          firmware_revision,
          model_number);
}

void ata_read(uint16_t *buffer, uint16_t bus, uint32_t lba, uint32_t sector_count)
{
    outb(bus + ATA_OFFSET_DRIVE, 0xE0 | (uint8_t)((lba >> 24) & 0x0F));

    outb(bus + ATA_OFFSET_FEATURES, 0);
    outb(bus + ATA_OFFSET_SECTOR_COUNT, sector_count);
    outb(bus + ATA_OFFSET_LBA0, (uint8_t)lba);
    outb(bus + ATA_OFFSET_LBA1, (uint8_t)(lba >> 8));
    outb(bus + ATA_OFFSET_LBA2, (uint8_t)(lba >> 16));

    outb(bus + ATA_OFFSET_COMMAND, ATA_COMMAND_READ);

    debug("read, sector=%d, count=%d, buffer=%x", lba, sector_count, ((uint32_t)buffer));

    for (size_t i = 0; i < sector_count; i++)
    {
        ata_wait(bus);
        ata_poll(bus);

        for (size_t j = 0; j < 256; j++)
        {
            // TODO: Read bytes per sector from disk parameters.
            buffer[i * 256 + j] = inw(bus);
        }
    }
}

void ata_init()
{
    ata_identify(ATA_BUS_PRIMARY, ATA_DISK_PRIMARY);
    ata_identify(ATA_BUS_PRIMARY, ATA_DISK_SECONDARY);
}

void ata_string_swap(string buffer, uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i += 2)
    {
        buffer[i] = data[i + 1];
        buffer[i + 1] = data[i];
    }
}