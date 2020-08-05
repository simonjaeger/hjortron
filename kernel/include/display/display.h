#ifndef DISPLAY_DISPLAY_H_
#define DISPLAY_DISPLAY_H_

#include <stdint.h>
#include "string.h"

typedef struct text_attribute
{
    uint8_t background;
    uint8_t foreground;
} text_attribute;

void display_init();
void display_destroy();
void display_clear();

void putc(const char c);
void puts(string str);

// TODO: Share implementation with sprintf? How to deal with the buffer...?
void printf(const string str, ...);

uint32_t display_test();

#endif // DISPLAY_DISPLAY_H_