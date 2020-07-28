#include "cpu/irq.h"
#include "cpu/io.h"
#include "cpu/pic.h"
#include "debug.h"

static idt_entry entries[IDT_SIZE] = {0};
static void (*handlers[IDT_SIZE])() = {0};

extern void irq0x00();
extern void irq0x01();
extern void irq0x02();
extern void irq0x03();
extern void irq0x04();
extern void irq0x05();
extern void irq0x06();
extern void irq0x07();
extern void irq0x08();
extern void irq0x0A();
extern void irq0x0B();
extern void irq0x0C();
extern void irq0x0D();
extern void irq0x0E();
extern void irq0x10();
extern void irq0x11();
extern void irq0x12();
extern void irq0x13();
extern void irq0x14();
extern void irq0x1E();

extern void irq0x20();
extern void irq0x21();
extern void irq0x22();
extern void irq0x23();
extern void irq0x24();
extern void irq0x25();
extern void irq0x26();
extern void irq0x27();
extern void irq0x28();
extern void irq0x29();
extern void irq0x2A();
extern void irq0x2B();
extern void irq0x2C();
extern void irq0x2D();
extern void irq0x2E();
extern void irq0x2F();

extern void irq_ignore();

extern void irq_handle(uint8_t irq)
{
    pic_eoi(irq);

    if (handlers[irq] != NULL)
    {
        handlers[irq]();
    }
}

void irq_init()
{
    // By default every interrupt is initialized but ignored.
    uint16_t segment = 0x08;
    for (size_t i = 0; i < IDT_SIZE; i++)
    {
        init_idt_entry(&entries[i], segment, irq_ignore, 0, IDT_GATE_TYPE_INTERRUPT_32);
    }

    init_idt_entry(&entries[0x00], segment, irq0x00, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x01], segment, irq0x01, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x02], segment, irq0x02, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x03], segment, irq0x03, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x04], segment, irq0x04, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x05], segment, irq0x05, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x06], segment, irq0x06, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x07], segment, irq0x07, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x08], segment, irq0x08, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0A], segment, irq0x0A, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0B], segment, irq0x0B, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0C], segment, irq0x0C, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0D], segment, irq0x0D, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x0E], segment, irq0x0E, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x10], segment, irq0x10, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x11], segment, irq0x11, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x12], segment, irq0x12, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x13], segment, irq0x13, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x14], segment, irq0x14, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x1E], segment, irq0x1E, 0, IDT_GATE_TYPE_INTERRUPT_32);

    init_idt_entry(&entries[0x20], segment, irq0x20, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x21], segment, irq0x21, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x22], segment, irq0x22, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x23], segment, irq0x23, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x24], segment, irq0x24, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x25], segment, irq0x25, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x26], segment, irq0x26, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x27], segment, irq0x27, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x28], segment, irq0x28, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x29], segment, irq0x29, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2A], segment, irq0x2A, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2B], segment, irq0x2B, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2C], segment, irq0x2C, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2D], segment, irq0x2D, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2E], segment, irq0x2E, 0, IDT_GATE_TYPE_INTERRUPT_32);
    init_idt_entry(&entries[0x2F], segment, irq0x2F, 0, IDT_GATE_TYPE_INTERRUPT_32);

    // Load interrupt descriptor table register.
    idt idtr;
    idtr.size = IDT_SIZE * sizeof(idt_entry) - 1;
    idtr.entries = &entries[0];

    asm volatile("lidt %0" ::"m"(idtr));

    debug("%s", "initialized");
}

void irq_enable()
{
    asm("sti");

    debug("%s", "enabled");
}

void irq_disable()
{
    asm("cli");

    debug("%s", "disabled");
}

void irq_init_handler(uint8_t irq, void *(handler))
{
    handlers[irq] = handler;

    debug("initialized handler, irq=%x, handler=%lx", irq, (uint32_t)handler);
}

void irq_terminate_handler(uint8_t irq)
{
    handlers[irq] = NULL;

    debug("terminated handler, irq=%x", irq);
}