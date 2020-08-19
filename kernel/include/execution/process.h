#ifndef EXECUTION_PROCESS_H_
#define EXECUTION_PROCESS_H_

#include <stdint.h>
#include <stddef.h>

#define PROCESS_STATE_WAITING 0x0
#define PROCESS_STATE_RUNNING 0x1
#define PROCESS_STATE_SLEEPING 0x2

typedef struct process
{
    uint32_t id;
    uint32_t *esp;
    uint32_t *ebp;
    uint32_t *eip;
    uint8_t state;
    size_t sleep;
} process_t;

process_t *process_create(uint32_t *eip);
void process_destroy(process_t *process);

#endif // EXECUTION_PROCESS_H_