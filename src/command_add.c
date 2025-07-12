#include "db.h"
#include <stdio.h>
#include "fmt.h"
#include <string.h>
#include <stddef.h>
#include <time.h>
#include "log.h"
#include "datetime.h"

int64_t absolute_time(char * buffer);
int64_t parse_different_time_formats(char * buffer, size_t char_count);

void command_add() {
    // ------ Get title ------
    char title[50] = { 0 };
    log_info("add_notification: Starting add notification\n");
    pretty_print("Adding notification\n");

    pretty_print("What should the title be? (gets truncated at 50 characters)\n");
    int len = 0;
    for(; len < 50; len++) {
        int c = getchar();
        if(c == (int)'\n' || c == EOF) break;
        title[len] = (char)c;
    }

    // -- Get due datetime -----
    pretty_print("When should it be due?\n");
    pretty_print("Formats:\n");
    //pretty_print("- UNIX time: 1234u\n");
    pretty_print("- Absolute time: 12d24m56y12h34m\n");
    pretty_print("- Delta time: +12d24m56y12h34m\n");

    int64_t due_datetime = -1;

    while(due_datetime == -1) {
        char buffer[64] = { 0 }; // Arbitrary size
        while(NULL == fgets(buffer, sizeof(buffer), stdin)) {
            pretty_print("Error getting input, try again\n");
        }

        size_t len = strlen(buffer);

        // Remove \n
        if(buffer[len - 1] == '\n') {
            buffer[len - 1] = 0;
            len--;
        }

        due_datetime = parse_different_time_formats(buffer, len);
        log_debug("Unix time: %ld\n", due_datetime);
    }

    // ----- Get creation time -----
    int64_t creation_datetime = time(NULL);
    log_debug("Unix time: %ld\n", creation_datetime);
}

int64_t parse_different_time_formats(char * buffer, size_t len) {
    int64_t result = 0;

    // Unix time
    /*if(buffer[char_count - 1] == 'u') {
        log_debug("Probably entered unix time\n");
        if(sscanf(buffer, "%ldu", &result) == 1) {
            return result;
        }
    }*/

    // Delta time
    //int day, month, year, hour, minute;
    //if(sscanf(buffer, "+%dd%dm%dy%dh%dm", &day, &month, &year, &hour, &minute) == 5) {
    //    if(day < 0 || month < 0 || year < 0 || hour < 0 || minute < 0) {
    //        pretty_print("One of the entered values if negative, this is not allowed, try again");
    //        return -1;
    //    }
    //}
    if(buffer[0] == '+') {
        // Maybe this:
        // Convert current time to unix time
        // Convert deltatime directly to unix time (no offset needed because of delta)
        // Add them together
        TODO("Delta time is not implemented yet");
    }

    return absolute_time(buffer);
}

int64_t absolute_time(char * buffer) {
    const char * format = "%dd%dm%dy%dh%dm";
    int day, month, year, hour, minute;
    if(sscanf(buffer, format, &day, &month, &year, &hour, &minute) == 5) {
        datetime_t dt = { 0 };
        if(datetime_new(&dt, minute, hour, day, month, year) == -1) return -1;

        time_t t = datetime_from(&dt);

        if(t == -1) {
            pretty_print("Failed to parse, try again\n");
            return -1;
        }

        return t;
    }
    pretty_print("Invalid format, try again\n");
    return -1;
}