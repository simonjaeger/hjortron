#include <stdint.h>
#include <stddef.h>
#include "drivers/pit.h"
#include "cpu/io.h"
#include "cpu/irq.h"
#include "execution/scheduler.h"
#include "debug.h"

#define PIT_CHANNEL0_DATA 0x40
#define PIT_CHANNEL1_DATA 0x41
#define PIT_CHANNEL2_DATA 0x42
#define PIT_COMMAND 0x43

static size_t ticks = 0;

void pit_handle_irq(const regs *r)
{
    ticks++;
    scheduler_handle_irq((regs *)r);
}

void pit_init()
{
    /* Configure PIT with square wave, without BCD. */
    outb(PIT_COMMAND, 0x36);

    uint16_t data = PIT_OSCILLATOR_HZ / PIT_INTERRUPTS_PER_SECOND;
    outb(PIT_CHANNEL0_DATA, data & 0xFF);
    outb(PIT_CHANNEL0_DATA, data >> 8);

    irq_init_handler(0x20, pit_handle_irq);

    info("%s", "initialized");
}