#ifndef EXECUTION_PROCESS_H_
#define EXECUTION_PROCESS_H_

#include <stdint.h>

typedef struct process
{
    uint32_t id;
    uint32_t *esp;
    uint32_t *ebp;
    uint32_t *eip;
} process_t;

process_t *process_create(uint32_t *eip);
void process_destroy(process_t *process);

#endif // EXECUTION_PROCESS_H_