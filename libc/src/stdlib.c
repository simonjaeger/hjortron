#include "stdlib.h"
#include "syscall.h"

uint32_t lib_test(void)
{
    return syscall_test();
}

void exit(uint32_t status)
{
    syscall_exit(status);
}