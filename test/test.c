#include <stdio.h>
#include <assert.h>

#include "datetime.h"

#define YEAR 2025
#define MONTH 1
#define DAY 1
#define HOUR 12
#define MINUTE 0
#define SECONDS_PER_DAY 60 * 60 * 24

void test_time() {
    const datetime_t DT = { 0 };
    datetime_t * x = (datetime_t *)&DT;
    datetime_new(x, MINUTE, HOUR, DAY, MONTH, YEAR);

    /* Test conversion */
    time_t t = datetime_from(&DT);

    datetime_t dt = { 0 };
    datetime(&dt, &t);

    assert(memcmp(&DT, &dt, sizeof(datetime_t)) == 0);

    /* Test add function */
    memcpy(&dt, &DT, sizeof(datetime_t));
    datetime_add_minutes(&dt, 2 * 60);

    assert(DT.year == dt.year);
    assert(DT.month == dt.month);
    assert(DT.day == dt.day);
    assert(DT.hour + 2 == dt.hour);
    assert(DT.minute == dt.minute);

    /* Invalid time */
    datetime_new(&dt, 0, 0, 40, 1, 2025);
    assert(datetime_from(&dt) == 0);

    datetime_new(&dt, 0, 0, 29, 2, 2025);
    assert(datetime_from(&dt) == 0);
}

int main() {
    test_time();
}