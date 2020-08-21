#include "syscall.h"

uint32_t syscall_test(void)
{
    uint32_t value = 0;
    asm volatile("int $0x80" ::"a"(0xFF), "b"(&value));
    return value;
}

void syscall_exit(uint32_t status)
{
    asm volatile("int $0x80" ::"a"(0x3C), "b"(status));
}