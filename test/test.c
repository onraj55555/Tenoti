#include <stdio.h>
#include <assert.h>

#include "date.h"

int main() {
    time_t t = 1019167200; // 19/04/2002 00:00:00
    datetime_t dt;
    datetime(&dt, &t);

    assert(dt.year == 2002);
    assert(dt.month == 4);
    assert(dt.days = 19);
    assert(dt.hours = 0);
    assert(dt.minutes = 0);
}