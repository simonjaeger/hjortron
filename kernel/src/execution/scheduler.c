#include <stdbool.h>
#include "execution/scheduler.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "debug.h"
#include "assert.h"
#include "list.h"
#include "execution/process.h"

static list_t *list;
static process_t *current;
static bool enabled;

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
        printf("%da ", i++);
        sleep();
    }
}

void task2()
{
    size_t i = 0;
    while (true)
    {
        printf("%db ", i++);
        sleep();
    }
}

void scheduler_init()
{
    list = list_create();
    current = NULL;
    enabled = false;

    list_insert(list, process_create((uint32_t *)&task1));
    list_insert(list, process_create((uint32_t *)&task2));

    info("%s", "initialized");
}

extern void *scheduler_iret;

void scheduler_handle_irq(regs *r)
{
    assert(r);

    if (!enabled)
    {
        return;
    }

    if (current == NULL)
    {
        current = list->head->value;
    }
    else
    {
        // Save current esp.
        current->esp = (uint32_t *)r;

        // Select next process.
        node_t *node = list_find(list, current);
        if (node->next == NULL)
        {
            current = list->head->value;
        }
        else
        {
            current = node->next->value;
        }
    }

    // Set esp and resume execution.
    asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    asm volatile("jmp isr_restore");
}

void scheduler_enable(void)
{
    enabled = true;
    info("%s", "enabled");
}

void scheduler_disable(void)
{
    enabled = false;
    info("%s", "disabled");
}