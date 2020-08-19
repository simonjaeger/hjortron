#include <stdbool.h>
#include "drivers/pci.h"
#include "cpu/io.h"
#include "memory/malloc.h"
#include "debug.h"

#define PCI_COMMAND_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

#define PCI_MAX_BUSES 256
#define PCI_MAX_DEVICES 32
#define PCI_MAX_FUNCTIONS 8

#define PCI_VENDOR_ID 0
#define PCI_DEVICE_ID 2

static pci_device *devices[0xFF] = {0};
static size_t device_count = 0;

uint32_t get_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t config_address = (uint32_t)bus << 16;
    config_address |= ((uint32_t)device) << 11;
    config_address |= ((uint32_t)function) << 8;
    config_address |= offset & 0xFC;
    config_address |= (uint32_t)0x80000000;
    return config_address;
}

uint32_t pci_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t config_address = get_config_address(bus, device, function, offset);
    outl(PCI_COMMAND_PORT, config_address);
    return inl(PCI_DATA_PORT);
}

uint16_t pci_device_id(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0);
    return data >> 16;
}

uint16_t pci_vendor_id(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0);
    return data & 0xFFFF;
}

uint16_t pci_status(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 4);
    return data >> 16;
}

uint16_t pci_command(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 4);
    return data & 0xFFFF;
}

uint8_t pci_class_code(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0x8);
    return (data >> 24) & 0xFF;
}

uint8_t pci_subclass(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0x8);
    return (data >> 16) & 0xFF;
}

uint8_t pci_prog_if(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0x8);
    return (data >> 8) & 0xFF;
}

uint8_t pci_revision_id(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0x8);
    return data;
}

uint8_t pci_bist(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0xC);
    return (data >> 24) & 0xFF;
}

uint8_t pci_header_type(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0xC);
    return (data >> 16) & 0xFF;
}

uint8_t pci_latency_timer(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0xC);
    return (data >> 8) & 0xFF;
}

uint8_t pci_cache_line_size(uint8_t bus, uint8_t device, uint8_t function)
{
    uint32_t data = pci_read(bus, device, function, 0xC);
    return data;
}

void pci_init()
{
    for (size_t b = 0; b < PCI_MAX_BUSES; b++)
    {
        for (size_t d = 0; d < PCI_MAX_DEVICES; d++)
        {
            uint32_t header_type = pci_header_type(b, d, 0);

            /* Check if the device is a multiple functions. */
            bool multifunction = header_type & (1 << 7);
            size_t functions = multifunction ? PCI_MAX_FUNCTIONS : 1;

            for (size_t f = 0; f < functions; f++)
            {
                /* Reuse the header type for the first function. */
                if (f > 0)
                {
                    header_type = pci_header_type(b, d, f);
                }

                uint16_t device_id = pci_device_id(b, d, f);
                uint16_t vendor_id = pci_vendor_id(b, d, f);

                if (!vendor_id || vendor_id == 0xFFFF)
                {
                    continue;
                }

                /* Create device. */
                pci_device *device = (pci_device *)malloc(sizeof(pci_device));
                device->bus = b;
                device->device = d;
                device->function = f;
                device->vendor_id = vendor_id;
                device->device_id = device_id;
                device->command = pci_command(b, d, f);
                device->status = pci_status(b, d, f);
                device->class_code = pci_class_code(b, d, f);
                device->subclass = pci_subclass(b, d, f);
                device->prog_if = pci_prog_if(b, d, f);
                device->revision_id = pci_revision_id(b, d, f);
                device->bist = pci_bist(b, d, f);
                device->header_type = header_type;
                device->latency_timer = pci_latency_timer(b, d, f);
                device->cache_line_size = pci_cache_line_size(b, d, f);

                devices[device_count] = device;
                device_count++;

                info("detected device, vendor_id=%x, device_id=%x, class_code=%x, subclass=%x",
                      device->vendor_id,
                      device->device_id,
                      device->class_code,
                      device->subclass);
            }
        }
    }
}