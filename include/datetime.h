#ifndef DATE_H_
#define DATE_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATE_FORMAT "%02d:%02d %02d/%02d/%d" // Only works untill 23:59 31/12/9999

typedef struct datetime_t datetime_t;

struct datetime_t {
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

void datetime_new(datetime_t * dt, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year);
char * datetime_to_string(const datetime_t * dt);
void datetime_print(const datetime_t * dt);
void datetime(datetime_t * dt, time_t * t);

/**
 * @brief Converts a datetime_t struct to UNIX time
 * 
 * @param dt datetime_t struct
 * @return The UNIX time representation of the datetime_t struct, 0 if the conversion errored
*/
time_t datetime_from(const datetime_t * dt);
int is_leapyear(uint16_t year);
// Watch out for dayligh savings!
void datetime_add_minutes(datetime_t * dt, unsigned long minutes);

uint8_t datetime_day_in_month(uint16_t year, uint8_t month);

#endif