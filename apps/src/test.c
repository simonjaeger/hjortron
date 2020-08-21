#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "display/display.h"
#include "stdlib.h"

int main()
{
    uint32_t data = lib_test();
    display_clear();
    printf("Hi! %x", data);
    return 0;
}