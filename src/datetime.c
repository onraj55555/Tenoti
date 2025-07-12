#include "datetime.h"
#include "log.h"
#include "fmt.h"

int months[12] = {31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 14:58 14/04/1999
char * datetime_to_string(const datetime_t * dt) {
    char * buffer = (char *) malloc(17 * sizeof(char));
    snprintf(buffer, 17, DATE_FORMAT, dt->hour, dt->minute, dt->day, dt->month, dt->year);
    return buffer;
}

void datetime_to_tm(const datetime_t * dt, struct tm * tm) {
    tm->tm_year = dt->year - 1900;
    tm->tm_mon = dt->month - 1;
    tm->tm_mday = dt->day;
    tm->tm_hour = dt->hour;
    tm->tm_min = dt->minute;
    tm->tm_sec = 0;
    // TODO: perhaps make this manual
    tm->tm_isdst = -1;
}

void tm_to_datetime(datetime_t * dt, const struct tm * tm) {
    dt->year = tm->tm_year + 1900;
    dt->month = tm->tm_mon + 1;
    dt->day = tm->tm_mday;
    dt->hour = tm->tm_hour;
    dt->minute = tm->tm_min;
}

void datetime(datetime_t * dt, time_t * t) {
    time_t tt;
    if(t == NULL) time(&tt);
    else tt = *t;

    struct tm * timeinfo;
    timeinfo = localtime(&tt);

    tm_to_datetime(dt, timeinfo);
}

int is_leapyear(uint16_t year) {
    if(year % 400 == 0) return 1;
    if(year % 100 == 0) return 0;
    if(year % 4 == 0) return 1;
    return 0;
}

time_t datetime_from(const datetime_t * dt) {
    struct tm tm;
    datetime_to_tm(dt, &tm);
    time_t t = mktime(&tm);
    return t;
}

void datetime_add_minutes(datetime_t * dt, unsigned long minutes) {
    time_t t = datetime_from(dt);
    t += minutes * 60;
    datetime(dt, &t);
}

int datetime_new(datetime_t * dt, uint8_t minute, uint8_t hour, uint8_t day, uint8_t month, uint16_t year) {
    if(year < 1900) {
        pretty_print("Year cannot be lower than 1900, try again\n");
        return -1;
    }

    if(month == 0 || month > 12) {
        pretty_print("Month must fall in [1, 12], try again\n");
    }

    int days_in_month = 1;
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            days_in_month = 31;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            days_in_month = 30;
            break;
        case 2:
            if(is_leapyear(year)) days_in_month = 29;
            else days_in_month = 28;
            break;
        default: panic("This shoud not happen\n");
    }

    if(day == 0 || day > days_in_month) {
        pretty_print("Day must fall in [1, %d], try again\n", days_in_month);
        return -1;
    }

    if(hour > 23) {
        pretty_print("Hour must fall in [0, 23], try again\n");
        return -1;
    }

    if(minute > 59) {
        pretty_print("Minute must fall in [0, 59], try again\n");
        return -1;
    }
    
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
    switch(month) {
        case 0:
        case 2:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
            return 31;
            break;
        case 3:
        case 5:
        case 8:
        case 10:
            return 30;
            break;
        case 1:
            if(is_leapyear(year)) return 29;
            return 28;
        // TODO: add default branch
    }
}
