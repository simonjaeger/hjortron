#include "cpu/irq.h"
#include "cpu/io.h"
#include "cpu/pic.h"
#include "debug.h"

static idt_entry entries[IDT_SIZE] = {0};
static void (*handlers[IDT_SIZE])(const regs *r) = {0};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();

extern void isr128();

extern void isr_ignore();

extern void irq_handler(const regs *r)
{
    pic_eoi(r->irq);

    if (handlers[r->irq] != NULL)
    {
        handlers[r->irq](r);
    }
}

void irq_init_idt(idt_entry *entry, uint8_t selector, void (*handler)(), uint8_t access, uint8_t type)
{
    uint32_t address = (uint32_t)handler;
    entry->offset0_15 = (address & 0xFFFF);
    entry->selector = selector;
    entry->reserved = 0;
    entry->flags = 0x80 | type | ((access & 3) << 5);
    entry->offset16_31 = (address >> 16) & 0xFFFF;
}

void irq_init()
{
    /* By default every interrupt is initialized but ignored. */
    uint16_t segment = 0x08;
    for (size_t i = 0; i < IDT_SIZE; i++)
    {
        irq_init_idt(&entries[i], segment, isr_ignore, 0, IDT_GATE_TYPE_INTERRUPT_32);
    }

    irq_init_idt(&entries[0], segment, isr0, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[1], segment, isr1, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[2], segment, isr2, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[3], segment, isr3, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[4], segment, isr4, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[5], segment, isr5, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[6], segment, isr6, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[7], segment, isr7, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[8], segment, isr8, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[9], segment, isr9, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[10], segment, isr10, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[11], segment, isr11, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[12], segment, isr12, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[13], segment, isr13, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[14], segment, isr14, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[15], segment, isr15, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[16], segment, isr16, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[17], segment, isr17, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[18], segment, isr18, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[19], segment, isr19, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[20], segment, isr20, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[21], segment, isr21, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[22], segment, isr22, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[23], segment, isr23, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[24], segment, isr24, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[25], segment, isr25, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[26], segment, isr26, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[27], segment, isr27, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[28], segment, isr28, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[29], segment, isr29, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[30], segment, isr30, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[31], segment, isr31, 0, IDT_GATE_TYPE_INTERRUPT_32);

    irq_init_idt(&entries[32], segment, isr32, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[33], segment, isr33, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[34], segment, isr34, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[35], segment, isr35, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[36], segment, isr36, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[37], segment, isr37, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[38], segment, isr38, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[39], segment, isr39, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[40], segment, isr40, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[41], segment, isr41, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[42], segment, isr42, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[43], segment, isr43, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[44], segment, isr44, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[45], segment, isr45, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[46], segment, isr46, 0, IDT_GATE_TYPE_INTERRUPT_32);
    irq_init_idt(&entries[47], segment, isr47, 0, IDT_GATE_TYPE_INTERRUPT_32);

    irq_init_idt(&entries[128], segment, isr128, 0, IDT_GATE_TYPE_INTERRUPT_32);

    /* Load interrupt descriptor table register. */
    idt idtr;
    idtr.size = IDT_SIZE * sizeof(idt_entry) - 1;
    idtr.entries = &entries[0];

    asm volatile("lidt %0" ::"m"(idtr));

    info("%s", "initialized");
}

void irq_enable()
{
    asm("sti");
    info("%s", "enabled");
}

void irq_disable()
{
    asm("cli");
    info("%s", "disabled");
}

void irq_init_handler(uint8_t irq, void (*handler)(const regs *r))
{
    handlers[irq] = handler;
    info("initialized handler, irq=%x, handler=%lx", irq, (uint32_t)handler);
}

void irq_terminate_handler(uint8_t irq)
{
    handlers[irq] = NULL;
    info("terminated handler, irq=%x", irq);
}