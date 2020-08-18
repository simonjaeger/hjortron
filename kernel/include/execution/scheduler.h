#ifndef EXECUTION_SCHEDULER_H_
#define EXECUTION_SCHEDULER_H_

#include "cpu/irq.h"

void scheduler_init();
void scheduler_handle_irq(regs *r);

#endif // EXECUTION_SCHEDULER_H_