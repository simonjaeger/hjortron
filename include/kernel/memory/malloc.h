#include "types.h"

void malloc_init(uint32_t heap_begin, uint32_t heap_size);

void *malloc(uint32_t size);
void free(void *ptr);