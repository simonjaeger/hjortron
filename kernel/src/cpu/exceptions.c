#include "cpu/exceptions.h"
#include "cpu/irq.h"
#include "string.h"
#include "debug.h"
#include "display/colors.h"
#include "display/display.h"

const string messages[] = 
{
    "divide by zero",
    "debug",
    "non maskable interrupt",
    "breakpoint",
    "overflow",
    "out of bounds",
    "invalid opcode",
    "device not available",
    "double fault",
    "reserved",
    "invalid tss",
    "invalid segment",
    "stack fault",
    "general protection fault",
    "page fault",
    "reserved",
    "x87 floating-point",
    "alignment check",
    "machine check",
    "SIMD floating-point",
    "virtualization",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "security",
    "reserved"
};

void exceptions_handle_irq(const regs *r)
{
    const string reason = messages[r->irq];
    printf("%fhalt, reason=%s", (text_attribute){COLOR_RED, COLOR_WHITE}, reason);
    error("halt, reason=%s", reason);
    asm("hlt");
}

void exceptions_init()
{
    for (size_t i = 0; i < 32; i++)
    {
        irq_init_handler(i, exceptions_handle_irq);
    }

    info("%s", "initialized");
}