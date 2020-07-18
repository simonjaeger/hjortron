#include "display/display.h"
#include "display/colors.h"
#include "cpu/io.h"

#define DISPLAY_MEMORY 0xB8000
#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 25

#define DEFAULT_BACKGROUND COLOR_BLACK
#define DEFAULT_FOREGROUND COLOR_LIGHT_GRAY

#define CURSOR_SCANLINE_START 0
#define CURSOR_SCANLINE_END 15

typedef struct text_data
{
    char code;
    uint8_t foreground : 4;
    uint8_t background : 4;
} __attribute__((packed)) text_data;

static text_data *buffer = (text_data *)DISPLAY_MEMORY;
static size_t position = 0;

void putcf(const char c, const text_attribute attribute)
{
    if (!c)
    {
        return;
    }

    if (c == '\n')
    {
        // Increment position to the next line.
        position += DISPLAY_WIDTH - (position % DISPLAY_WIDTH);
        set_cursor(position);
        return;
    }

    // Shift buffer if needed to show the next row.
    // TODO: Refactor and apply for new line as well.
    if ((size_t)(position + 1) > DISPLAY_WIDTH * DISPLAY_HEIGHT)
    {
        for (uint32_t i = 0; i < DISPLAY_WIDTH * (DISPLAY_HEIGHT - 1); i++)
        {
            buffer[i] = buffer[i + DISPLAY_WIDTH];
        }

        for (uint32_t i = DISPLAY_WIDTH * (DISPLAY_HEIGHT - 1); i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
        {
            buffer[i].code = 0;
            buffer[i].foreground = DEFAULT_FOREGROUND;
            buffer[i].background = DEFAULT_BACKGROUND;
        }

        position -= DISPLAY_WIDTH;
    }

    // Set text data.
    buffer[position].code = c;
    buffer[position].foreground = attribute.foreground;
    buffer[position].background = attribute.background;

    position++;
    set_cursor(position);
}

void putsf(string str, text_attribute attribute)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        putcf(str[i], attribute);
    }
}

void putc(const char c)
{
    putcf(c, (text_attribute){DEFAULT_BACKGROUND, DEFAULT_FOREGROUND});
}

void puts(string str)
{
    text_attribute attribute = (text_attribute){DEFAULT_BACKGROUND, DEFAULT_FOREGROUND};
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        putcf(str[i], attribute);
    }
}

void put_hex(size_t count)
{
    puts("0x");
    for (size_t i = 0; i < count; i++)
    {
        putc('0');
    }
}

void printf(const string str, ...)
{
    // Get variable list.
    __builtin_va_list ap;
    __builtin_va_start(ap, str);

    // Create default attribute.
    text_attribute attribute = {DEFAULT_BACKGROUND, DEFAULT_FOREGROUND};

    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++)
    {
        // Check if the next character is a formatted argument. Subsequently
        // the type of the formatted argument (%s, %d, ...) should follow.
        if (str[i] == '%' && i + 1 < len)
        {
            switch (str[i + 1])
            {
            case 's':
            {
                string arg = __builtin_va_arg(ap, string);
                putsf(arg, attribute);
            }
            break;
            case 'd':
            {
                // Convert argument to base 10.
                uint32_t arg = __builtin_va_arg(ap, uint32_t);
                char buffer[32];
                itoa(arg, buffer, 10);
                putsf(buffer, attribute);
            }
            break;
            case 'x':
            {
                // Convert argument to base 16.
                uint32_t arg = __builtin_va_arg(ap, uint32_t);
                char buffer[32];
                itoa(arg, buffer, 16);

                put_hex(8 - strlen(buffer));
                putsf(buffer, attribute);
            }
            break;
            case 'l':
            {
                if (i + 2 < len)
                {
                    switch (str[i + 2])
                    {
                    case 'd':
                    {
                        // Convert argument to base 10.
                        uint64_t arg = __builtin_va_arg(ap, uint64_t);
                        char buffer[32];
                        itoa(arg, buffer, 10);
                        putsf(buffer, attribute);
                    }
                    break;
                    case 'x':
                    {
                        // Convert argument to base 16.
                        uint64_t arg = __builtin_va_arg(ap, uint64_t);
                        char buffer[32];
                        itoa(arg, buffer, 16);

                        put_hex(16 - strlen(buffer));
                        putsf(buffer, attribute);
                    }
                    break;
                    default:
                        break;
                    }

                    i++;
                }
            }
            break;
            case 'f':
            {
                attribute = __builtin_va_arg(ap, text_attribute);
            }
            break;
            default:
                break;
            }

            i++;
            continue;
        }

        putcf(str[i], attribute);
    }

    __builtin_va_end(ap);
}

void clear()
{
    for (size_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++)
    {
        buffer[i].code = 0;
        buffer[i].foreground = DEFAULT_FOREGROUND;
        buffer[i].background = DEFAULT_BACKGROUND;
    }
    position = 0;
}

void enable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | CURSOR_SCANLINE_START);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | CURSOR_SCANLINE_END);
}

void disable_cursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void set_cursor(size_t position)
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}