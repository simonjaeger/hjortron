#include "device/serial.h"

#define SERIAL_DATA_BITS_5 0x00
#define SERIAL_DATA_BITS_6 0x01
#define SERIAL_DATA_BITS_7 0x02
#define SERIAL_DATA_BITS_8 0x03

void serial_init(uint32_t port)
{
    // Disable interrupts.
    outb(port + 1, 0x00);

    // Set speed of serial line.
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);

    // Set data bits.
    outb(port + 3, SERIAL_DATA_BITS_8);
}

int serial_ready(uint16_t port)
{
    return inb(port + 5) & 0x20;
}

void serial_putc(uint32_t port, char c)
{
    // Wait for transmission buffer to be empty.
    while (!serial_ready(port))
        ;
    outb(port, c);
}

void serial_puts(uint32_t port, string str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        serial_putc(port, str[i]);
    }
}