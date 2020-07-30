#ifndef GDT_H_
#define GDT_H_

#include <stdint.h>

typedef struct gdt_entry
{
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t access;
    uint8_t limit16_19 : 4;
    uint8_t flags : 4;
    uint8_t base24_31;
} __attribute__((packed)) gdt_entry;

typedef struct gdt
{
    uint16_t size;
    gdt_entry *entries;
} __attribute__((packed)) gdt;

#endif // GDT_H_