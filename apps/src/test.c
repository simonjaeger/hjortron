#include <stdint.h>
#include "string.h"
#include "display/display.h"

static const uint32_t data = 0x1234;

int main()
{
    display_clear();
    printf("Hi! %x", data);
    return 0;
}