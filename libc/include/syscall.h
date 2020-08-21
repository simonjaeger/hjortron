#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdint.h>

uint32_t syscall_test(void);
void syscall_exit(uint32_t status);

#endif // SYSCALL_H_