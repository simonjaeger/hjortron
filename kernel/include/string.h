#ifndef STRING_H_
#define STRING_H_

#include "types.h"

typedef char *string;

size_t strlen(const string str);
// u32 strcmp(const string str1, const string str2);
void strcpy(string src, string dest, size_t *offset);
void strtrim(string str, char c);

string reverse(string str, size_t i, size_t j);

// void atoi(string str, int *i);
string itoa(int32_t i, string str, size_t base);

void sprintf(string str, string format, ...);
void sprintf_va(string str, string format, __builtin_va_list ap);

#endif // STRING_H_