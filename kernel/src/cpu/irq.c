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
extern void isr30();

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

extern void isr_ignore();

extern void irq_handler(const regs *r)
{
    pic_eoi(r->irq);

    if (handlers[r->irq] != NULL)
    {
        handlers[r->irq](r);
    }
}

void irq_init()
{
    // By default every interrupt is initialized but ignored.
    uint16_t segment = 0x08;
    for (size_t i = 0; i < IDT_SIZE; i++)
    {
        init_idt_entry(&entries[i], segment, isr_ignore, 0, IDT_GATE_TYPE_INTERRUPT_32);
    }

    init_idt_entry(&entries[0x00], segment, isr0, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x01], segment, isr1, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x02], segment, isr2, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x03], segment, isr3, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x04], segment, isr4, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x05], segment, isr5, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x06], segment, isr6, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x07], segment, isr7, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x08], segment, isr8, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x08], segment, isr9, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0A], segment, isr10, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0B], segment, isr11, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0C], segment, isr12, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0D], segment, isr13, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0E], segment, isr14, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0E], segment, isr15, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x10], segment, isr16, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x11], segment, isr17, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x12], segment, isr18, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x13], segment, isr19, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x14], segment, isr20, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x1E], segment, isr30, 0, IDT_GATE_TYPE_INTERRUPT_32);

    init_idt_entry(&entries[0x20], segment, isr32, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x21], segment, isr33, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x22], segment, isr34, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x23], segment, isr35, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x24], segment, isr36, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x25], segment, isr37, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x26], segment, isr38, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x27], segment, isr39, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x28], segment, isr40, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x29], segment, isr41, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2A], segment, isr42, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2B], segment, isr43, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2C], segment, isr44, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2D], segment, isr45, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2E], segment, isr46, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2F], segment, isr47, 0, IDT_GATE_TYPE_INTERRUPT_32);

    // Load interrupt descriptor table register.
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