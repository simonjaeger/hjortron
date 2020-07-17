#ifndef CPU_IRQ_H_
#define CPU_IRQ_H_

#include "types.h"
#include "cpu/idt.h"

void irq_init();
void irq_enable();
void irq_disable();

void irq_init_handler(uint8_t irq, void *(handler));
void irq_terminate_handler(uint8_t irq);

#endif // CPU_IRQ_H_