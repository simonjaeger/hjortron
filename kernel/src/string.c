#include "string.h"
#include "stdarg.h"

size_t strlen(const string str)
{
    size_t i = 0;
    while (str[i])
    {
        i++;
    }
    return i;
}

size_t strcmp(const string str1, const string str2)
{
    string s1 = (string)str1;
    string s2 = (string)str2;
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void strcpy(string src, string dest, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        dest[i] = src[i];
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

void strset(string str, char c, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        str[i] = c;
    }
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
    va_list ap;
    va_start(ap, format);
    sprintf_va(str, format, ap);
    va_end(ap);
}

void sprintf_va(string str, string format, va_list ap)
{
    size_t j = 0;
    size_t len = strlen(format);
    for (size_t i = 0; i < len; i++)
    {
        // Copy standard characters.
        while (format[i] != '%' && i < len)
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
            size_t arg_len = strlen(arg);
            strcpy(arg, &str[j], arg_len);
            j += arg_len;
        }
        break;
        case 'd':
        {
            uint32_t arg = __builtin_va_arg(ap, uint32_t);
            char buffer[32];
            strset(buffer, '\0', 32);
            itoa(arg, buffer, 10);

            size_t arg_len = strlen(&buffer[0]);
            strcpy(buffer, &str[j], arg_len);
            j += arg_len;
        }
        break;
        case 'x':
        {
            uint32_t arg = __builtin_va_arg(ap, uint32_t);
            char buffer[32];
            strset(buffer, '\0', 32);
            itoa(arg, buffer, 16);

            size_t arg_len = strlen(&buffer[0]);
            strcpy("0x", &str[j], 2);
            strcpy(buffer, &str[j + 2], arg_len);
            j += arg_len + 2;
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
                strset(buffer, '\0', 32);
                itoa(arg, buffer, 10);

                size_t arg_len = strlen(&buffer[0]);
                strcpy(buffer, &str[j], arg_len);
                j += arg_len;
            }
            break;
            case 'x':
            {
                uint64_t arg = __builtin_va_arg(ap, uint64_t);
                char buffer[32];
                strset(buffer, '\0', 32);
                itoa(arg, buffer, 16);

                size_t arg_len = strlen(&buffer[0]);
                strcpy("0x", &str[j], 2);
                strcpy(buffer, &str[j + 2], arg_len);
                j += arg_len + 2;
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

void memcpy(void *dest, void *src, size_t len)
{
    char *src2 = (char *)src;
    char *dest2 = (char *)dest;

    for (size_t i = 0; i < len; i++)
    {
        dest2[i] = src2[i];
    }
}