#include <stdbool.h>
#include "execution/scheduler.h"
#include "memory/malloc.h"
#include "display/display.h"
#include "drivers/pit.h"
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

void scheduler_init(void)
{
    list = list_create();
    current = NULL;
    enabled = false;
    info("%s", "initialized");
}

process_t *scheduler_next()
{
    node_t *node = list->head;
    if (current != NULL)
    {
        node = list_find(list, current);
    }

    for (size_t i = 0; i < list->length; i++)
    {
        node = node->next;
        if (node == NULL)
        {
            node = list->head;
        }

        /* Check if process is waiting. */
        if (((process_t *)node->value)->state == PROCESS_STATE_WAITING)
        {
            return node->value;
        }
    }
    return NULL;
}

void scheduler_switch()
{
    /* Select next process. */
    current = scheduler_next();

    /* Switch stack and resume execution. */
    if (current == NULL)
    {
        assert(esp);
        asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)esp));
    }
    else if (current != NULL)
    {
        current->state = PROCESS_STATE_RUNNING;
        asm volatile("mov %%eax, %%esp" ::"a"((uint32_t)current->esp));
    }
    asm volatile("jmp isr_restore");
}

void scheduler_handle_irq(const regs *r)
{
    assert(r);

    if (!enabled)
    {
        return;
    }

    /* Decrement sleep counters. */
    for (node_t *node = list->head; node != NULL; node = node->next)
    {
        process_t *process = (process_t *)node->value;
        if (process->state == PROCESS_STATE_SLEEPING)
        {
            if (process->sleep == 0)
            {
                info("awake, id=%d", process->id);
                process->state = PROCESS_STATE_WAITING;
                continue;
            }
            process->sleep--;
        }
    }

    if (current == NULL)
    {
        /* 
         * Save kernel stack so that it can be restored
         * when there are no running processes.
         */
        esp = (uint32_t *)r;
    }
    else
    {
        /* Save process stack. */
        current->esp = (uint32_t *)r;
        current->state = PROCESS_STATE_WAITING;
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

void scheduler_start(process_t *process)
{
    assert(process);

    list_insert(list, process);
    info("start, id=%d", process->id);
}

void scheduler_stop(process_t *process)
{
    assert(process);

    list_delete(list, process);
    process_destroy(process);
    info("kill, id=%d", process->id);

    if (current == process)
    {
        current = NULL;
        scheduler_switch();
    }
}

void scheduler_sleep(process_t *process, size_t ms, const regs *r)
{
    assert(process);

    info("sleep, id=%d, ms=%d", process->id, ms);

    /* 
     * Compute ticks based on PIT configuration as scheduler
     * is dependent on PIT interrupts.
     */
    size_t ticks = ms * PIT_INTERRUPTS_PER_SECOND / 1000;

    process->state = PROCESS_STATE_SLEEPING;
    process->sleep = ticks;

    if (current == process)
    {
        /* Save process stack. */
        current->esp = (uint32_t *)r;

        current = NULL;
        scheduler_switch();
    }
}