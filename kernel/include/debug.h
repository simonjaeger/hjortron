#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>
#include <stddef.h>
#include "string.h"

#define DEBUG_SERIAL

#define DEBUG_TYPE_TRACE 0x1
#define DEBUG_TYPE_DEBUG 0x2
#define DEBUG_TYPE_INFO 0x3
#define DEBUG_TYPE_WARN 0x4
#define DEBUG_TYPE_ERROR 0x5

void dprintf(size_t type, string file, const char *function, size_t line, string format, ...);

#define trace(str, ...) dprintf(DEBUG_TYPE_TRACE, __FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)
#define debug(str, ...) dprintf(DEBUG_TYPE_DEBUG, __FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)
#define info(str, ...) dprintf(DEBUG_TYPE_INFO, __FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)
#define warn(str, ...) dprintf(DEBUG_TYPE_WARN, __FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)
#define error(str, ...) dprintf(DEBUG_TYPE_ERROR, __FILE__, __FUNCTION__, __LINE__, str, __VA_ARGS__)

#endif // DEBUG_H_