#include <stdbool.h>
#include "scheduler.h"
#include "cpu/irq.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "debug.h"
#include "assert.h"
#include "execution/process.h"

// TODO: Create list_t.

#define MAX_THREADS 256
#define THREAD_STACK_LENGTH 2048

static process_t *threads[MAX_THREADS];
static process_t *current;
static uint32_t next_id;

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

void scheduler_init()
{
    for (size_t i = 0; i < MAX_THREADS; i++)
    {
        threads[i] = NULL;
    }

    current = NULL;
    next_id = 0;

    // TODO: Remove.
    threads[0] = process_create((uint32_t *)&task1);
    threads[1] = process_create((uint32_t *)&task2);

    info("%s", "initialized");
}

void scheduler_schedule(regs *r)
{
    // TODO: Create list_t.
    assert(r);

    if (current == NULL)
    {
        current = threads[0];
    }
    else
    {
        current->esp = (uint32_t *)r;

        if (current == threads[1])
        {
            current = threads[0];
        }
        else
        {
            current = threads[1];
        }
    }

    asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    asm volatile("jmp scheduler_iret");
}