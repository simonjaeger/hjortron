#ifndef CPU_IDT_H_
#define CPU_IDT_H_

#include <stdint.h>

#define IDT_SIZE 0xFF

#define IDT_GATE_TYPE_TASK 0x5
#define IDT_GATE_TYPE_INTERRUPT_16 0x6
#define IDT_GATE_TYPE_TRAP_16 0x7
#define IDT_GATE_TYPE_INTERRUPT_32 0xE
#define IDT_GATE_TYPE_TRAP_32 0xF

typedef struct idt_entry
{
    uint16_t offset0_15;
    uint16_t selector;
    uint8_t reserved;
    uint8_t flags;
    uint16_t offset16_31;
} __attribute__((packed)) idt_entry;

typedef struct idt
{
    uint16_t size;
    idt_entry *entries;
} __attribute__((packed)) idt;

void init_idt_entry(idt_entry *entry, uint8_t selector, void (*handler)(), uint8_t access, uint8_t type);

#endif // CPU_IDT_H_