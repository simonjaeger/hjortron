#ifndef DRIVERS_CMOS_H_
#define DRIVERS_CMOS_H_

#include <stdint.h>

typedef struct time_t
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} time_t;

time_t cmos_time();

#endif // DRIVERS_CMOS_H_