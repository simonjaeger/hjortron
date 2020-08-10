#ifndef DRIVERS_PCI_H_
#define DRIVERS_PCI_H_

#include <stdint.h>

#define PCI_GENERAL_DEVICE 0x00
#define PCI_BRIDGE_DEVICE 0x01
#define PCI_CARD_BUS_BRIDGE_DEVICE 0x02

typedef struct pci_device
{
    uint16_t bus;
    uint16_t device;
    uint16_t function;
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t status;
    uint16_t command;
    uint8_t class_code;
    uint8_t subclass;
    uint8_t prog_if;
    uint8_t revision_id;
    uint8_t bist;
    uint8_t header_type;
    uint8_t latency_timer;
    uint8_t cache_line_size;
} pci_device;

void pci_init();

#endif // DRIVERS_PCI_H_