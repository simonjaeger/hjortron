#include <stdint.h>
#include "execution/syscall.h"
#include "execution/scheduler.h"
#include "cpu/irq.h"
#include "assert.h"
#include "debug.h"
#include "filesystem/fs.h"

void syscall_handle_irq(const regs *r)
{
    assert(r->irq == 0x80);
    info("handle, id=%x", r->eax);

    process_t *process = scheduler_process();

    /* Type of syscall. */
    switch (r->eax)
    {
    case SYSCALL_STOP:
    case SYSCALL_EXIT:
        scheduler_stop(process);
        break;
    case SYSCALL_SLEEP:
        scheduler_sleep(process, (size_t)r->ebx, r);
        break;
    case SYSCALL_TEST:
        *((uint32_t *)r->ebx) = 0x1234;
        break;

    default:
        error("unsupported, id=%x", r->eax);
        break;
    }
}

void syscall_init()
{
    irq_init_handler(0x80, syscall_handle_irq);
    info("%s", "initialized");
}