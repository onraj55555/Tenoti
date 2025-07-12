#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    setenv("TZ", "Europe/Brussels", 1);
    tzset();

    struct tm tm = { 0 };
    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 19;
    tm.tm_mon = 4;
    tm.tm_year = 2002 - 1900;
    tm.tm_isdst = 1;

    time_t t =  mktime(&tm);

    printf("%ld\n", t);

    struct tm * local = localtime(&t);

    printf("%d %d %d %d %d %d\n", local->tm_year + 1900, local->tm_mon, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
}
