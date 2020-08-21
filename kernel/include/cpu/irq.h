#ifndef CPU_IRQ_H_
#define CPU_IRQ_H_

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

typedef struct regs
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t irq;
    uint32_t err;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} regs;

void irq_init();
void irq_enable();
void irq_disable();

void irq_init_handler(uint8_t irq, void (*handler)(const regs *r));
void irq_terminate_handler(uint8_t irq);

#endif // CPU_IRQ_H_