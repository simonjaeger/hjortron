#include <stdbool.h>
#include "scheduler.h"
#include "cpu/irq.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "debug.h"
#include "assert.h"

// TODO: Create list_t.

#define MAX_THREADS 256
#define THREAD_STACK_LENGTH 2048

static thread *threads[MAX_THREADS];
static thread *current;

void sleep()
{
    for (size_t i = 0; i < 10000000; i++)
    {
        if (1 + 1)
        {
            continue;
        }
    }
}

void task1()
{
    size_t i = 0;
    while (true)
    {
        // debug("%d", 1);
        printf("%da ", i++);
        sleep();
    }
}

void task2()
{
    size_t i = 0;
    while (true)
    {
        // debug("%d", 1);
        printf("%db ", i++);
        sleep();
    }
}

thread *task_create(uint32_t *eip)
{
    // Allocate thread.
    thread *t;
    for (size_t i = 0; i < MAX_THREADS; i++)
    {
        if (threads[i] == NULL)
        {
            threads[i] = (thread *)malloc(sizeof(thread));
            t = threads[i];
            t->idx = i;
            break;
        }
    }

    assert(t);

    t->eip = eip;

    // Allocate stack.
    uint8_t *stack = (uint8_t *)malloc(THREAD_STACK_LENGTH);
    assert(stack);

    t->ebp = (uint32_t *)(stack + THREAD_STACK_LENGTH);
    t->esp = (uint32_t *)(stack + THREAD_STACK_LENGTH - sizeof(regs));

    // Create stack.
    regs *r = (regs *)t->esp;
    r->gs = 0x10;
    r->fs = 0x10;
    r->es = 0x10;
    r->ds = 0x10;

    r->edi = 0;
    r->esi = 0;

    r->ebp = (uint32_t)t->ebp;
    r->esp = (uint32_t)t->esp;

    r->ebx = 0;
    r->edx = 0;
    r->ecx = 0;
    r->eax = 0;

    r->irq = 0x20;
    r->err = 0;

    r->eip = (uint32_t)t->eip;
    r->cs = 0x8;
    r->eflags = 0x206; // TODO: Confirm.
    r->useresp = 0;
    r->ss = 0;

    info("created, ebp=%x, esp=%x, eip=%x", t->ebp, t->esp, t->eip);

    return t;
}

void scheduler_init()
{
    for (size_t i = 0; i < MAX_THREADS; i++)
    {
        threads[i] = NULL;
    }

    current = NULL;

    // TODO: Remove.
    thread *t1 = task_create((uint32_t *)&task1);
    thread *t2 = task_create((uint32_t *)&task2);

    info("%s", "initialized");

    debug("ADDR1 %x %x", t1->esp, t2->esp);
    debug("ADDR2 %x %x", &task1, &task2);
}

void scheduler_schedule(regs *r)
{
    // TODO: Create list_t.
    assert(r);

    if (current == NULL)
    {
        debug("TEST: %x %x", r->ebp, r->esp);

        current = threads[0];

        // r->ebp = (uint32_t)current->ebp;
        // r->esp = (uint32_t)current->esp;
        // r->eip = (uint32_t)current->eip;

        asm volatile("mov %%eax, %%ebp" ::"a"((uint32_t)current->ebp));
        asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
        asm volatile("jmp scheduler_iret");

        // uint32_t t = 0;
        // asm volatile("mov %%esp, %%eax"
        //              : "=a"(t));
        // debug("TEST3: %x %x %x", t, r, (uint32_t)r - t);

        // asm volatile("pop %gs");
        // asm volatile("pop %fs");
        // asm volatile("pop %es");
        // asm volatile("pop %ds");

        // asm volatile("popad");

        // asm volatile("add esp, $8");
        // asm volatile("pop %ebp");
        // asm volatile("pop %esp");

        // asm volatile("pop %esp");

        return;
    }

    current->esp = (uint32_t *)r;

    // debug("TEST2 %x %x %x", current->idx, current->esp, r->esp);
    debug("%x %x", current->esp, (uint32_t *)r->esp);

    // // // current->ebp = (uint32_t *)r->ebp;
    // // // current->eip = (uint32_t *)r->eip;

    if (current == threads[1])
    {
        current = threads[0];
    }
    else
    {
        current = threads[1];
    }

    asm volatile("mov %%eax, %%ebp" ::"a"((uint32_t)current->ebp));
    asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    asm volatile("jmp scheduler_iret");

    // // debug("te %d %x", current->idx, current->esp);

    // // // r->ebp = (uint32_t)current->ebp;
    // r->esp = (uint32_t)current->esp;
    // // // r->eip = (uint32_t)current->eip;
}