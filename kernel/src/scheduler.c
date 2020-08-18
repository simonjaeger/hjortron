#include <stdbool.h>
#include "scheduler.h"
#include "cpu/irq.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "debug.h"
#include "assert.h"
#include "list.h"
#include "execution/process.h"

static list_t *list;
static process_t *current;

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

    list_insert(list, process_create((uint32_t *)&task1));
    list_insert(list, process_create((uint32_t *)&task2));

    info("%s", "initialized");
}

void scheduler_schedule(regs *r)
{
    assert(r);

    if (current == NULL)
    {
        current = list->head->value;
    }
    else
    {
        current->esp = (uint32_t *)r;

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

    asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    asm volatile("jmp scheduler_iret");
}