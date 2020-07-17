#include "string.h"

size_t strlen(const string str)
{
    size_t i = 0;
    while (str[i])
    {
        i++;
    }
    return i;
}

void swap(char *a, char *b)
{
    char t = *a;
    *a = *b;
    *b = t;
}

string reverse(string str, size_t i, size_t j)
{
    while (i < j)
    {
        swap(&str[i++], &str[j--]);
    }
    return str;
}

void strcpy(string src, string dest, size_t *offset)
{
    size_t len = strlen(src);
    for (size_t i = 0; i < len; i++)
    {
        dest[*offset] = src[i];
        *offset = *offset + 1;
    }
}

void strtrim(string str, char c)
{
    size_t len = strlen(str);
    for (int32_t i = len - 1; i >= 0; i--)
    {
        if (str[i] == c)
        {
            str[i] = '\0';
            continue;
        }
        break;
    }

    // TODO: Trim start.
}

string itoa(int32_t i, string str, size_t base)
{
    // Check if base is valid.
    if (base < 2 || base > 32)
    {
        return str;
    }

    // Only handle negative numbers for base 10. Standard
    // behavior of itoa() function.
    bool negative = i < 0 && base == 10;
    if (negative)
    {
        i *= -1;
    }

    size_t j = 0;
    while (i)
    {
        size_t r = i % base;
        // Check if a character should be added, otherwise add a digit.
        if (r >= 10)
        {
            str[j++] = 65 + r - 10;
        }
        else
        {
            str[j++] = 48 + r;
        }

        i = i / base;
    }

    // Handle zero explicitly.
    if (j == 0)
    {
        str[j++] = '0';
    }
    else if (negative)
    {
        str[j++] = '-';
    }

    // Add null terminator.
    str[j] = '\0';

    // Reverse string.
    return reverse(str, 0, j - 1);
}

void sprintf(string str, string format, ...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap, format);
    sprintf_va(str, format, ap);
    __builtin_va_end(ap);
}

void sprintf_va(string str, string format, __builtin_va_list ap)
{
    size_t j = 0;
    size_t len = strlen(format);
    for (size_t i = 0; i < len; i++)
    {
        // Copy standard characters.
        while (format[i] != '%')
        {
            str[j++] = format[i++];
        }

        // Check for missing format type.
        if (++i >= len)
        {
            // TODO: Handle error.
            continue;
        }

        switch (format[i])
        {
        case 'c':
        {
            char arg = (char)__builtin_va_arg(ap, int);
            str[j++] = arg;
        }
        break;
        case 's':
        {
            string arg = __builtin_va_arg(ap, string);
            strcpy(arg, str, &j);
        }
        break;
        case 'd':
        {
            uint32_t arg = __builtin_va_arg(ap, uint32_t);
            char buffer[32];
            itoa(arg, buffer, 10);
            strcpy(buffer, str, &j);
        }
        break;
        case 'x':
        {
            uint32_t arg = __builtin_va_arg(ap, uint32_t);
            char buffer[32];
            itoa(arg, buffer, 16);
            strcpy("0x", str, &j);
            strcpy(buffer, str, &j);
        }
        break;
        case 'l':
        {
            // Check for missing format type.
            if (++i >= len)
            {
                // TODO: Handle error.
                continue;
            }

            switch (format[i])
            {
            case 'd':
            {
                uint64_t arg = __builtin_va_arg(ap, uint64_t);
                char buffer[32];
                itoa(arg, buffer, 10);
                strcpy(buffer, str, &j);
            }
            break;
            case 'x':
            {
                uint64_t arg = __builtin_va_arg(ap, uint64_t);
                char buffer[32];
                itoa(arg, buffer, 16);
                strcpy("0x", str, &j);
                strcpy(buffer, str, &j);
            }
            break;
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
    }

    str[j] = '\0';
}