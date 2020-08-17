#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>

typedef struct thread
{
    uint32_t idx;
    uint32_t *esp;
    uint32_t *ebp;
    uint32_t *eip;
} thread;

void scheduler_init();
void scheduler_schedule();

#endif // SCHEDULER_H_