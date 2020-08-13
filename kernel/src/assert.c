#include "assert.h"
#include "display/colors.h"
#include "display/display.h"
#include "debug.h"

void _assert(string file, const char *function, size_t line, string expression)
{
    printf("%fhalt, reason=%s", (text_attribute){COLOR_RED, COLOR_WHITE}, expression);
    dprintf(DEBUG_TYPE_ERROR, file, function, line, "halt, reason=%s", expression);
    asm("hlt");
}