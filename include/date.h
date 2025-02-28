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
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t month;
    uint16_t year;
};

// 14:58 14/04/1999
char * date_to_string(const datetime_t * datetime) {
    char * buffer = (char *) malloc(17 * sizeof(char));
    snprintf(buffer, 17, DATE_FORMAT, datetime->hours, datetime->minutes, datetime->days, datetime->month, datetime->year);
    return buffer;
}

void datetime(datetime_t * datetime, time_t * t) {
    time_t tt;
    if(t == NULL) time(&tt);
    else tt = *t;

    struct tm * timeinfo;
    timeinfo = localtime(&tt);

    datetime->year = timeinfo->tm_year + 1900; // 1900 corresponds with 0
    datetime->month = timeinfo->tm_mon + 1; // Zero based
    datetime->days = timeinfo->tm_mday;
    datetime->hours = timeinfo->tm_hour;
    datetime->minutes = timeinfo->tm_min;
}

time_t date_from(const datetime_t * datetime) {
    char * str = date_to_string(datetime);

    struct tm tm;

    memset(&tm, 0, sizeof(struct tm));

    char * s = strptime(str, "%H:%M %d/%m/%Y", &tm);

    if(s == NULL) {
        printf("Parsing error\n");
        exit(1);
    }

    time_t t = mktime(&tm);

    free(str);

    return t;
}

#endif