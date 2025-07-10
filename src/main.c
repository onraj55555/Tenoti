#define _GNU_SOURCE
#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#include "db.h"
#include "log.h"
#include "sql.h"
#include "fmt.h"


//#define CREATED_FILE "/home/onraj/.tenoti/created"
#define CREATED_FILE "db/created"

//#define FIRST_DUE_FILE "/home/onraj/.tenoti/first_due"
#define FIRST_DUE_FILE "db/first_due"

#define FUTURE_TIME 2 * 60 * 60 // max 2 hrs in advance

// Used timer, global in case testing is needed
timer_t * TIMER = NULL;

int64_t get_current_time();
int64_t from_now(int64_t t);
int is_leapyear(int year);

void general_usage(sqlite3 * db);
void parse_args(sqlite3 * db, int argc, char ** argv);

void add_notification(sqlite3 * db);

void panic(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stdout, fmt, args);

    va_end(args);

    db_close();
    exit(EXIT_FAILURE);
}

int main(int argc, char ** argv) {
    db_init();

    if(!db) {
        log_error("Main: DB is NULL\n");
        exit(EXIT_FAILURE);
    }

    notification_t n = { 0 };
    const char * title = "This is a title";
    memcpy(n.title, title, strlen(title));
    n.created_at = get_current_time();
    n.due = get_current_time() + FUTURE_TIME + 10;
    n.checked = 0;
    n.reminder = 0;
    n.details = 0;

    db_insert_into_notifications(&n);

    // No params, check if possible overdue and check DB if needed
    if(argc == 1) {
        general_usage(db);
    } else {
        log_debug("Parsing arguments\n");
        parse_args(db, argc - 1, argv + 1);
    }

    db_close();

    return 0;
}



int64_t get_current_time() {
    return time(TIMER);
}

int64_t from_now(int64_t t) {
    return t - get_current_time();
}

int is_leapyear(int year) {
    if(year % 4 == 0) {
        if(year % 100) {
            if(year % 400) return 1;
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

void general_usage(sqlite3 * db) {
    // Get all notifications which might be due
    // A.k.a. check which aren't checked yet and their due <= current_time + FUTURE_TIME

    log_info("General usage: Start\n");

    int64_t max_due = get_current_time() + FUTURE_TIME;

    notification_list_t list = { 0 };

    log_info("General usage: Getting all next unchecked notifications before time\n");

    int r = db_find_all_notifications_unchecked_before_time(&list, max_due);
    //int r = db_find_all_notifications(db, &list);
    log_info("General usage: Done getting them\n");

    if(r == -1) {
        log_error("General usage: An error occured while getting all due notifications\n");

        free_list(&list);

        return;
    }

    notification_link_t * link = list.notifications_link;
    while(link) {
        char * buffer = notification_to_str(&link->notification);
        pretty_print("%s\n", buffer);
        link = link->next;
    }

    free_list(&list);
}

// argv starts from the global argv starting from index 1
void parse_args(sqlite3 * db, int argc, char ** argv) {
    // Add notification
    if(strcmp(argv[0], "add") == 0 || argv[0][0] == 'a') {
        log_info("parse_args: Add argument found\n");
        add_notification(db);
    }
}

int64_t parse_different_time_formats(char * buffer, ssize_t char_count);

void add_notification(sqlite3 * db) {
    notification_t n = { 0 };
    log_info("add_notification: Starting add notification\n");
    pretty_print("Adding notification\n");

    pretty_print("What should the title be? (gets truncated at 50 characters)\n");
    // TODO: change magic number to define macro in schemas.c
    fgets(n.title, 51, stdin);

    pretty_print("When should it be due?\n");
    pretty_print("Formats:\n");
    //pretty_print("- UNIX time: 1234u\n");
    pretty_print("- Delta time: +12d24m56y12h34m\n");
    pretty_print("- Absolute time: 12d24m56y12h34m\n");

    int64_t entered_time = -1;

    while(entered_time == -1) {
        char * buffer = 0;
        size_t buffersize = 0;
        ssize_t char_count = getline(&buffer, &buffersize, stdin);

        // Remove \n
        if(buffer[char_count - 1] == '\n') {
            buffer[char_count - 1] = 0;
            char_count--;
        }

        entered_time = parse_different_time_formats(buffer, char_count);
        printf("Unix time: %ld\n", entered_time);
    }
}

int64_t absolute_time(char * buffer) {
    const char * format = "%dd%dm%dy%dh%dm";
    int day, month, year, hour, minute;
    if(sscanf(buffer, format, &day, &month, &year, &hour, &minute) == 5) {

        if(year < 1970) {
            pretty_print("Year cannot be lower than 1970, try again\n");
            return -1;
        }

        if(month > 12) {
            pretty_print("Month value must fall in [1, 12], try again\n");
            return -1;
        }

        int days_in_month = 1;
        switch (month) {
            case 0:
            case 2:
            case 4:
            case 6:
            case 7:
            case 9:
            case 11:
                days_in_month = 31;
                break;
            case 3:
            case 5:
            case 8:
            case 10:
                days_in_month = 30;
                break;
            case 1:
                if(is_leapyear(year)) days_in_month = 29;
                else days_in_month = 28;
            default: panic("This shoud not happen\n");
        }

        if(day > days_in_month) {
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


        // Convert to unix time
        // VERKEERD
        struct tm tm;
        tm.tm_sec = 0;
        tm.tm_min = minute;
        tm.tm_hour = hour;
        tm.tm_mday = day;
        tm.tm_mon = month;
        tm.tm_year = year - 1970;
        tm.tm_isdst = -1; // Determines daylightsaving itself
        // TODO: mayby change this in the future because this might not be correct, see man mktime -> caveats
        return mktime(&tm);
    }
    return -1;
}

int64_t parse_different_time_formats(char * buffer, ssize_t char_count) {
    int64_t result = 0;

    // Unix time
    if(buffer[char_count - 1] == 'u') {
        log_debug("Probably entered unix time\n");
        if(sscanf(buffer, "%ldu", &result) == 1) {
            return result;
        }
    }

    // Delta time
    //int day, month, year, hour, minute;
    //if(sscanf(buffer, "+%dd%dm%dy%dh%dm", &day, &month, &year, &hour, &minute) == 5) {
    //    if(day < 0 || month < 0 || year < 0 || hour < 0 || minute < 0) {
    //        pretty_print("One of the entered values if negative, this is not allowed, try again");
    //        return -1;
    //    }
    //}
    if(buffer[0] == '+') {
        log_debug("Probably entering delta time\n");
        panic("Delta time is not implemented yet\n");
        //return non_unix_time(buffer, "+%dd%dm%dy%dh%dm");
    }

    log_debug("Probable entering absolute time\n");
    return absolute_time(buffer);
}
