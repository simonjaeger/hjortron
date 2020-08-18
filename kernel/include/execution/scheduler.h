#ifndef EXECUTION_SCHEDULER_H_
#define EXECUTION_SCHEDULER_H_

#include "cpu/irq.h"

void scheduler_init();
void scheduler_handle_irq(regs *r);
void scheduler_enable(void);
void scheduler_disable(void);

#endif // EXECUTION_SCHEDULER_H_