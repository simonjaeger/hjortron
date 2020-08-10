#include <stdarg.h>
#include "debug.h"
#include "display/display.h"
#include "device/serial.h"

#ifdef DEBUG_SERIAL
#define DEBUG_FORMATTED
#define dputc(c) serial_putc(SERIAL_COM1, c)
#define dputs(s) serial_puts(SERIAL_COM1, s)
#else
#define dputc putc
#define dputs puts
#endif

#define DEBUG_TYPE_TRACE 0x1
#define DEBUG_TYPE_DEBUG 0x2
#define DEBUG_TYPE_INFO 0x3
#define DEBUG_TYPE_WARN 0x4
#define DEBUG_TYPE_ERROR 0x5

#define BUFFER_LENGTH 1024

void dprintf(size_t type, string file, __attribute__((unused)) const char *function, size_t line, string format, ...)
{
    char buffer[1024];
    strset(buffer, '\0', 1024);

    // Print prefix.
    string type_str = "";
    switch (type)
    {
    case DEBUG_TYPE_TRACE:
#ifdef DEBUG_FORMATTED
        type_str = "\e[1;34mTRACE\e[0m";
#else
        type_str = "TRACE";
#endif
        break;
    case DEBUG_TYPE_DEBUG:
#ifdef DEBUG_FORMATTED
        type_str = "\e[1;34mDEBUG\e[0m";
#else
        type_str = "DEBUG";
#endif
        break;
    case DEBUG_TYPE_INFO:
#ifdef DEBUG_FORMATTED
        type_str = "\e[1;34mINFO\e[0m";
#else
        type_str = "INFO";
#endif
        break;
    case DEBUG_TYPE_WARN:
#ifdef DEBUG_FORMATTED
        type_str = "\e[1;93mWARN\e[0m";
#else
        type_str = "WARN";
#endif
        break;
    case DEBUG_TYPE_ERROR:
#ifdef DEBUG_FORMATTED
        type_str = "\e[1;31mERROR\e[0m";
#else
        type_str = "ERROR";
#endif
        break;
    default:
        break;
    }

    sprintf(buffer, "00:00:00 %s %s:%d: ", type_str, file, line);
    dputs(buffer);

    // Format string.
    va_list ap;
    va_start(ap, format);
    sprintf_va(buffer, format, ap);
    va_end(ap);

#ifdef DEBUG_FORMATTED
    dputs("\e[1m");
#endif
    // Print string.
    dputs(buffer);
#ifdef DEBUG_FORMATTED
    dputs("\e[0m\n");
#else
    dputc('\n');
#endif
}