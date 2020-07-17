#ifndef CPU_IO_H_
#define CPU_IO_H_

#include "types.h"

static inline void outb(uint16_t port, uint8_t data)
{
    asm volatile("outb %0, %1" ::"a"(data), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t data)
{
    asm volatile("outw %0, %1" ::"a"(data), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t data)
{
    asm volatile("outl %0, %1" ::"a"(data), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t data;
    asm volatile("inb %1, %0"
                 : "=a"(data)
                 : "Nd"(port));
    return data;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t data;
    asm volatile("inw %1, %0"
                 : "=a"(data)
                 : "Nd"(port));
    return data;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t data;
    asm volatile("inl %1, %0"
                 : "=a"(data)
                 : "Nd"(port));
    return data;
}

static inline void wait(void)
{
    asm volatile("jmp 1f\n\t"
                 "1:jmp 2f\n\t"
                 "2:");
}

#endif // CPU_IO_H_