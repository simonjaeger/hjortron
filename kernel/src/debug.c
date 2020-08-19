#include <stdarg.h>
#include "debug.h"
#include "display/display.h"
#include "drivers/serial.h"
#include "drivers/cmos.h"

#ifdef DEBUG_SERIAL
#define DEBUG_FORMATTED
#define dputc(c) serial_putc(SERIAL_COM1, c)
#define dputs(s) serial_puts(SERIAL_COM1, s)
#else
#define dputc(c) putc(c)
#define dputs(s) puts(s)
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

    /* Print prefix. */
    string type_str = "";
    switch (type)
    {
#ifdef DEBUG_FORMATTED
    case DEBUG_TYPE_TRACE:
        type_str = "\e[1;34mTRACE\e[0m";
        break;
    case DEBUG_TYPE_DEBUG:
        type_str = "\e[1;33mDEBUG\e[0m";
        break;
    case DEBUG_TYPE_INFO:
        type_str = "\e[1;34mINFO\e[0m";
        break;
    case DEBUG_TYPE_WARN:
        type_str = "\e[1;33mWARN\e[0m";
        break;
    case DEBUG_TYPE_ERROR:
        type_str = "\e[1;31mERROR\e[0m";
        break;
#else
    case DEBUG_TYPE_TRACE:
        type_str = "TRACE";
        break;
    case DEBUG_TYPE_DEBUG:
        type_str = "DEBUG";
        break;
    case DEBUG_TYPE_INFO:
        type_str = "INFO";
        break;
    case DEBUG_TYPE_WARN:
        type_str = "WARN";
        break;
    case DEBUG_TYPE_ERROR:
        type_str = "ERROR";
        break;
#endif
    default:
        break;
    }

    /* Print time. */
    time_t time = cmos_time();
    sprintf(buffer, time.hour < 10 ? "0%d:" : "%d:", time.hour);
    dputs(buffer);
    sprintf(buffer, time.minute < 10 ? "0%d:" : "%d:", time.minute);
    dputs(buffer);
    sprintf(buffer, time.second < 10 ? "0%d " : "%d ", time.second);
    dputs(buffer);

    /* Print debug info. */
    sprintf(buffer, "%s %s:%d: ", type_str, &file[4], line);
    dputs(buffer);

    /* Print string. */
    va_list ap;
    va_start(ap, format);
    sprintf_va(buffer, format, ap);
    va_end(ap);

#ifdef DEBUG_FORMATTED
    dputs("\e[1m");
#endif
    dputs(buffer);
#ifdef DEBUG_FORMATTED
    dputs("\e[0m\n");
#else
    dputc('\n');
#endif
}