#include "cpu/pic.h"
#include "cpu/io.h"
#include "debug.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC1_OFFSET 0x20

#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC2_OFFSET 0x28
#define PIC_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04

#define ICW4_8086 0x01

void pic_init()
{
    // Initialize.
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    wait();

    // Set new offsets for interrupts.
    outb(PIC1_DATA, PIC1_OFFSET);
    wait();
    outb(PIC2_DATA, PIC2_OFFSET);
    wait();

    // Set identity.
    outb(PIC1_DATA, ICW1_SINGLE);
    wait();
    outb(PIC2_DATA, ICW1_INTERVAL4);
    wait();

    // Set 8086/88 (MCS-80/85) mode.
    outb(PIC1_DATA, ICW4_8086);
    wait();
    outb(PIC2_DATA, ICW4_8086);
    wait();

    // Set masks.
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);

    info("initialized, pic1=%x, pic2=%x", PIC1_OFFSET, PIC2_OFFSET);
}

void pic_eoi(uint8_t irq)
{
    // Acknowledge interrupts within range.
    if (irq >= PIC1_OFFSET && irq <= (PIC2_OFFSET + 7))
    {
        outb(PIC1_COMMAND, PIC_EOI);

        if (irq >= PIC2_OFFSET)
        {
            outb(PIC2_COMMAND, PIC_EOI);
        }
    }
}
