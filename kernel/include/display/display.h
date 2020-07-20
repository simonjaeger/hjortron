#ifndef DISPLAY_DISPLAY_H_
#define DISPLAY_DISPLAY_H_

#include "types.h"
#include "string.h"

typedef struct text_attribute
{
    uint8_t background;
    uint8_t foreground;
} text_attribute;

void putc(const char c);
void puts(string str);

// TODO: Share implementation with sprintf? How to deal with the buffer...?
void printf(const string str, ...);

void clear();

void enable_cursor();
void disable_cursor();
void set_cursor(size_t position);

#define info printf
#define warn printf
#define error printf

#endif // DISPLAY_DISPLAY_H_