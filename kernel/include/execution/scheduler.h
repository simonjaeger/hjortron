#ifndef EXECUTION_SCHEDULER_H_
#define EXECUTION_SCHEDULER_H_

#include "cpu/irq.h"
#include "execution/process.h"

void scheduler_init(void);
void scheduler_handle_irq(regs *r);
void scheduler_enable(void);
void scheduler_disable(void);

process_t *scheduler_process(void);
void scheduler_kill(process_t *process);

#endif // EXECUTION_SCHEDULER_H_