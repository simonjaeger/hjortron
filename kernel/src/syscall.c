#include <stdint.h>
#include "syscall.h"
#include "cpu/irq.h"
#include "assert.h"
#include "debug.h"
#include "filesystem/fs.h"

void syscall_handle_irq(const regs *r)
{
    assert(r->irq == 0x80);
    info("handle, idx=%d", r->eax);

    // Type of syscall.
    switch (r->eax)
    {
    case SYSCALL_TEST:
        *((uint32_t*)r->ebx) = 0xFFFF;
        break;

    default:
        error("unsupported, idx=%d", r->eax);
        break;
    }
}

void syscall_init()
{
    irq_init_handler(0x80, syscall_handle_irq);
    info("%s", "initialized");
}