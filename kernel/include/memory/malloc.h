#ifndef MEMORY_MALLOC_H_
#define MEMORY_MALLOC_H_

#include <stdint.h>
#include "cpu/boot.h"

void malloc_init(const memory_map *mmap);

void *malloc(const uint32_t len);
void free(void *ptr);

#endif // MEMORY_MALLOC_H_