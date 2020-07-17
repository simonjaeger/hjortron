#ifndef CPU_PIC_H_
#define CPU_PIC_H_

#include "types.h"

void pic_init();
void pic_eoi(uint8_t irq);

#endif // CPU_PIC_H_