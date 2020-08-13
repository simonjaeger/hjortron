#ifndef ASSERT_H_
#define ASSERT_H_

#include "string.h"

void _assert(string file, const char *function, size_t line, string expression);

#define assert(expression) ((expression) ? (void)0 : _assert( __FILE__, __FUNCTION__, __LINE__, #expression))

#endif // ASSERT_H_