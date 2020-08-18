#include <stdbool.h>
#include "execution/scheduler.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "debug.h"
#include "assert.h"
#include "list.h"
#include "execution/process.h"
#include "execution/mutex.h"
#include "execution/syscall.h"

static list_t *list;
static process_t *current;
static bool enabled;
static uint32_t *esp;

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

        if (i > 25)
        {
            asm volatile("int $0x80" ::"a"(SYSCALL_KILL), "b"(0));
        }
    }
}

void task2()
{
    size_t i = 0;
    while (true)
    {
        printf("%db ", i++);
        sleep();

        if (i > 50)
        {
            asm volatile("int $0x80" ::"a"(SYSCALL_KILL), "b"(0));
        }
    }
}

void scheduler_init(void)
{
    list = list_create();
    current = NULL;
    enabled = false;

    list_insert(list, process_create((uint32_t *)&task1));
    list_insert(list, process_create((uint32_t *)&task2));

    info("%s", "initialized");
}

void scheduler_switch()
{
    // Switch stack and resume execution.
    if (current == NULL)
    {
        assert(esp);
        asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)esp));
    }
    else if (current != NULL)
    {
        asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    }
    asm volatile("jmp isr_restore");
}

void scheduler_handle_irq(regs *r)
{
    assert(r);

    if (!enabled || list->length == 0)
    {
        return;
    }

    if (current == NULL)
    {
        // Save kernel stack so that it can be restored
        // when there are no running processes.
        esp = (uint32_t *)r;

        current = list->head->value;
    }
    else
    {
        // Save process stack.
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

    scheduler_switch();
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

process_t *scheduler_process(void)
{
    return current;
}

void scheduler_kill(process_t *process)
{
    assert(process);

    list_delete(list, process);
    process_destroy(process);
    info("%s", "kill");

    // if (list->length == 0)
    // {
    //     debug("%s %x", "wut?", test_esp);
    //     current = NULL;
    //     asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)test_esp));
    //     return;
    // }

    // Select a new process if the current one is
    // being killed.
    if (current == process)
    {
        if (list->length > 0)
        {
            current = list->head->value;
        }
        else
        {
            current = NULL;
        }
        scheduler_switch();
    }
}