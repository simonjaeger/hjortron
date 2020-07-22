#include "debug.h"
#include "display/display.h"
#include "device/serial.h"

// // Use display as debug output.
// #define dputc putc
// #define dputs puts

// Use serial as debug output.
#define dputc(c) serial_putc(SERIAL_COM1, c)
#define dputs(s) serial_puts(SERIAL_COM1, s)

void debug_printf(string file, __attribute__((unused)) const char *function, size_t line, string format, ...)
{
    char buffer[256];
    strset(buffer, '\0', 256);

    // Print prefix.
    sprintf(buffer, "(%s:%d): ", file, line);
    dputs(buffer);

    // Format string.
    __builtin_va_list ap;
    __builtin_va_start(ap, format);
    sprintf_va(buffer, format, ap);
    __builtin_va_end(ap);

    // Print string.
    dputs(buffer);
    dputc('\n');
}