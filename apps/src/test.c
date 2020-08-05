#include <stdint.h>
#include "string.h"
#include "display/display.h"

__attribute__((unused)) static const uint32_t data1;
__attribute__((unused)) static const uint32_t data2 = 0x1234;

int main()
{
    // display_test(); 

    display_clear();
    printf("TEST.ELF started %x %x", data1, data2);

    // return display_test();
    // return (uint32_t)(display_test);

    uint32_t result = (uint32_t)(&display_test);
    result = display_test();
    return result;
    return (uint32_t)(&display_test);
}