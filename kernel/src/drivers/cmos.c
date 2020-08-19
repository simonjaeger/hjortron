#include <stdbool.h>
#include "drivers/cmos.h"
#include "cpu/io.h"
#include "debug.h"

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

#define CMOS_RTC_SECOND 0x00
#define CMOS_RTC_MINUTE 0x02
#define CMOS_RTC_HOUR 0x04
#define CMOS_RTC_DAY 0x07
#define CMOS_RTC_MONTH 0x08
#define CMOS_RTC_YEAR 0x09

bool cmos_wait()
{
    outb(CMOS_ADDRESS, 0x0A);
    return inb(CMOS_DATA) & 0x80;
}

uint8_t cmos_read(size_t offset)
{
    outb(CMOS_ADDRESS, offset);
    return inb(CMOS_DATA);
}

void cmos_write(size_t offset, uint8_t value)
{
    outb(CMOS_ADDRESS, offset);
    outb(CMOS_DATA, value);
}

time_t cmos_time()
{
    time_t time;

    while (cmos_wait())
        ;

    /* Read RTC registers. */
    time.second = cmos_read(CMOS_RTC_SECOND);
    time.minute = cmos_read(CMOS_RTC_MINUTE);
    time.hour = cmos_read(CMOS_RTC_HOUR);
    time.day = cmos_read(CMOS_RTC_DAY);
    time.month = cmos_read(CMOS_RTC_MONTH);
    time.year = cmos_read(CMOS_RTC_YEAR);

    uint8_t status = cmos_read(0xB);

    /* Convert BCD to binary. */
    if (!(status & 0x4))
    {
        time.second = (time.second & 0xF) + ((time.second / 16) * 10);
        time.minute = (time.minute & 0xF) + ((time.minute / 16) * 10);
        time.hour = ((time.hour & 0xF) + (((time.hour & 0x70) / 16) * 10)) + (time.hour & 0x80);
        time.day = (time.day & 0xF) + ((time.day / 16) * 10);
        time.month = (time.month & 0xF) + ((time.month / 16) * 10);
        time.year = (time.year & 0xF) + ((time.year / 16) * 10);
    }

    /* Convert 12 hour clock to 24 hour clock. */
    if (!(status & 0x02) && (time.hour & 0x80))
    {
        time.hour = ((time.hour & 0x7F) + 12) % 24;
    }
    return time;
}