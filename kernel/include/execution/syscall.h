#ifndef EXECUTION_SYSCALL_H_
#define EXECUTION_SYSCALL_H_

#define SYSCALL_STOP 0x1
#define SYSCALL_SLEEP 0x23
#define SYSCALL_TEST 0xFF

void syscall_init();

#endif // EXECUTION_SYSCALL_H_