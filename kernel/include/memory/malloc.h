#ifndef MEMORY_MALLOC_H_
#define MEMORY_MALLOC_H_

#include <stdint.h>

void malloc_init(uint32_t heap_begin, uint32_t heap_size);

void *malloc(uint32_t size);
void free(void *ptr);

#endif // MEMORY_MALLOC_H_