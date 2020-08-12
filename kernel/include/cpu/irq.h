#ifndef CPU_IRQ_H_
#define CPU_IRQ_H_

#include <stdint.h>
#include "cpu/idt.h"

typedef struct regs
{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t irq;
    uint32_t err;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} regs;

void irq_init();
void irq_enable();
void irq_disable();

void irq_init_handler(uint8_t irq, void *(handler));
void irq_terminate_handler(uint8_t irq);

#endif // CPU_IRQ_H_