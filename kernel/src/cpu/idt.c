#include "cpu/idt.h"

void init_idt_entry(idt_entry *entry, uint8_t selector, void (*handler)(), uint8_t access, uint8_t type)
{
    uint32_t address = (uint32_t)handler;
    entry->offset0_15 = (address & 0xFFFF);
    entry->selector = selector;
    entry->reserved = 0;
    entry->flags = 0x80 | type | ((access & 3) << 5);
    entry->offset16_31 = (address >> 16) & 0xFFFF;
}