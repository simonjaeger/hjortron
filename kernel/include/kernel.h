#ifndef KERNEL_H_
#define KERNEL_H_

#include "boot.h"

void main(const boot_info *boot_info) __attribute__((section(".text.main")));

#endif // KERNEL_H_