#ifndef EXECUTION_SCHEDULER_H_
#define EXECUTION_SCHEDULER_H_

#include "cpu/irq.h"
#include "execution/process.h"

void scheduler_init(void);
void scheduler_handle_irq(const regs *r);
void scheduler_enable(void);
void scheduler_disable(void);

process_t *scheduler_process(void);
void scheduler_start(process_t *process);
void scheduler_stop(process_t *process);
void scheduler_sleep(process_t *process, size_t ms, const regs *r);

#endif // EXECUTION_SCHEDULER_H_