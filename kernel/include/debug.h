#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include "string.h"

void debug_printf(string file, const char* function,  size_t line, string format, ...);
#define debug(str, ...) debug_printf(__FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)

#endif // DEBUG_H_