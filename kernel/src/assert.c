#include "assert.h"
#include "display/colors.h"
#include "display/display.h"
#include "debug.h"

void _assert(string file, const char *function, size_t line, string expression)
{
    printf("%fhalt, reason=%s, expression=%s", (text_attribute){COLOR_RED, COLOR_WHITE}, "assert", expression);
    dprintf(DEBUG_TYPE_ERROR, file, function, line, "halt, reason=%s, expression=%s", "assert", expression);
    asm("hlt");
}