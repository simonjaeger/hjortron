#ifndef SERIAL_H_
#define SERIAL_H_

#include "types.h"
#include "string.h"
#include "cpu/io.h"

#define SERIAL_COM1 0x3F8
#define SERIAL_COM2 0x2F8
#define SERIAL_COM3 0x3E8
#define SERIAL_COM4 0x2E8

void serial_init(uint32_t port);
void serial_putc(uint32_t port, char c);
void serial_puts(uint32_t port, string str);

#endif // SERIAL_H_