#include "datetime.h"
#include "log.h"

int months[12] = {31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 14:58 14/04/1999
char * datetime_to_string(const datetime_t * dt) {
    char * buffer = (char *) malloc(17 * sizeof(char));
    snprintf(buffer, 17, DATE_FORMAT, dt->hour, dt->minute, dt->day, dt->month, dt->year);
    return buffer;
}

void datetime(datetime_t * dt, time_t * t) {
    time_t tt;
    if(t == NULL) time(&tt);
    else tt = *t;

    struct tm * timeinfo;
    timeinfo = localtime(&tt);

    dt->year = timeinfo->tm_year + 1900; // 1900 corresponds with 0
    dt->month = timeinfo->tm_mon + 1; // Zero based
    dt->day = timeinfo->tm_mday;
    dt->hour = timeinfo->tm_hour;
    dt->minute = timeinfo->tm_min;
}

int is_leapyear(uint16_t year) {
    if(year % 400 == 0) return 1;
    if(year % 100 == 0) return 0;
    if(year % 4 == 0) return 1;
    return 0;
}

time_t datetime_from(const datetime_t * dt) {
    char * str = datetime_to_string(dt);

    log_debug("datetime_from(): %s\n", str);

    struct tm tm;

    memset(&tm, 0, sizeof(struct tm));

    char * s = strptime(str, "%H:%M %d/%m/%Y", &tm);

    if(s == NULL) {
        log_warn("datetime_from(): Invalid datetime entered '%s'\n", str);
        free(str);
        return 0;
    }

    time_t t = mktime(&tm);

    free(str);

    return t;
}

void datetime_add_minutes(datetime_t * dt, unsigned long minutes) {
    time_t t = datetime_from(dt);
    t += minutes * 60;
    datetime(dt, &t);
}

void datetime_new(datetime_t * dt, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year) {
    dt->year = year;
    dt->month = month;
    dt->day = day;
    dt->hour = hour;
    dt->minute = minute;
}

void datetime_print(const datetime_t * dt) {
    char * str = datetime_to_string(dt);
    printf("%s\n", str);
    free(str);
}

uint8_t datetime_day_in_month(uint16_t year, uint8_t month) {
    if(month == 2) {
        if(year % 4 == 0) {
            if(year % 100 == 0) {
                if(year % 400 == 0) { return 29; }
                else { return 28; }
            } else { return 29; }
        } else { return 28; }
    }
    return months[month - 1];
}