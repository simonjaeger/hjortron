#include "execution/process.h"
#include "cpu/irq.h"
#include "memory/malloc.h"
#include "assert.h"
#include "debug.h"

#define PROCESS_STACK_LENGTH 8192

static uint32_t next_id = 0;

process_t *process_create(uint32_t *eip)
{
    assert(eip);

    process_t *process = (process_t *)malloc(sizeof(process_t));
    assert(process);

    // Allocate stack.
    uint8_t *stack = (uint8_t *)malloc(PROCESS_STACK_LENGTH);
    assert(stack);

    process->id = next_id;
    process->ebp = (uint32_t *)(stack + PROCESS_STACK_LENGTH);
    process->esp = (uint32_t *)(stack + PROCESS_STACK_LENGTH - sizeof(regs));
    process->eip = eip;

    // Create stack.
    regs *r = (regs *)process->esp;

    // TODO: Get from GDT.
    r->gs = 0x10;
    r->fs = 0x10;
    r->es = 0x10;
    r->ds = 0x10;

    r->edi = 0;
    r->esi = 0;

    r->ebp = (uint32_t)process->ebp;
    r->esp = (uint32_t)process->esp;

    r->ebx = 0;
    r->edx = 0;
    r->ecx = 0;
    r->eax = 0;

    r->irq = 0x20;
    r->err = 0;

    r->eip = (uint32_t)process->eip;
    
    // TODO: Get from GDT.
    r->cs = 0x8;

    // TODO: Confirm.
    r->eflags = 0x206;
    r->useresp = 0;
    r->ss = 0;

    info("created, ebp=%x, esp=%x, eip=%x", process->ebp, process->esp, process->eip);
    return process;
}

void process_destroy(process_t *process)
{
    assert(process);

    free(process);
}
