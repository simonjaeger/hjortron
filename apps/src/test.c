#include <stdint.h>
#include "string.h"
#include "display/display.h"

static const uint32_t data1;
static const uint32_t data2 = 0x1234;

int main()
{
    printf("Hello! %3 %x", data1, data2);
    return data2;
}