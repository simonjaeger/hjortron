#ifndef STRING_H_
#define STRING_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

typedef char *string;

size_t strlen(const string str);
size_t strcmp(const string str1, const string str2);
void strcpy(string src, string dest, size_t len);
void strtrim(string str, char c);
void strset(string str, char c, size_t len);

string reverse(string str, size_t i, size_t j);

// void atoi(string str, int *i);
string itoa(int32_t i, string str, size_t base);

void sprintf(string str, string format, ...);
void sprintf_va(string str, string format, va_list ap);

void memcpy(void *dest, void *src, size_t len);

#endif // STRING_H_