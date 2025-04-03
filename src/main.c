#define _GNU_SOURCE
#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "db.h"
#include "log.h"
#include "sql.h"
#include "fmt.h"
#include "datetime.h"



void add_notification(sqlite3 * db);

int main(int argc, char ** argv) {
    datetime_t now = { 0 };
    datetime(&now, NULL);
    char * now_str = datetime_to_string(&now);

    printf("%s", now_str);

    return 0;

    sqlite3 * db = init();

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

    db_insert_into_notifications(db, &n);

    // No params, check if possible overdue and check DB if needed
    if(argc == 1) {
        general_usage(db);
    } else {
        pretty_print("Parsing args...\n");
    }

    db_close(db);

    return 0;
}

// Create tables if needed in case it's the first tine the code starts up
sqlite3 * init() {
    log_info("Init: Initializing DB\n");

    int exists = access(DB_FILE, F_OK);

    sqlite3 * db = NULL;
    db_open(DB_FILE, &db);

    if(exists != 0) {
        log_info("Init: Creating DB\n");

        if(!db) {
            log_error("Init: Failed to create DB file\n");
        }

        sqlite3_stmt * stmt;
        int rc = sqlite3_prepare_v2(db, CREATE_DETAILS_TABLE, -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            log_error("Init: Failed to create details table prepared statement\n");
            db_close(db);
            exit(EXIT_FAILURE);
        }

        if(db_exec(db, stmt, NULL) == -1) {
            log_error("Init: Failed to create details table\n");
            db_close(db);
            exit(EXIT_FAILURE);
        }

        rc = sqlite3_prepare_v2(db, CREATE_NOTIFICATIONS_TABLE, -1, &stmt, NULL);

        if(rc != SQLITE_OK) {
            log_error("Init: Failed to create notifications table prepared statement\n");
            db_close(db);
            exit(EXIT_FAILURE);
        }

        if(db_exec(db, stmt, NULL) == -1) {
            log_error("Init: Failed to create notifications table\n");
            db_close(db);
            exit(EXIT_FAILURE);
        }
    }

    if(!db) {
        log_error("Init: Failed to open DB\n");
    }

    return db;
}

int64_t get_current_time() {
    return time(NULL);
}

int64_t from_now(int64_t t) {
    return t - get_current_time();
}

void general_usage(sqlite3 * db) {
    // Get all notifications which might be due
    // A.k.a. check which aren't checked yet and their due <= current_time + FUTURE_TIME

    log_info("General usage: Start\n");

    int64_t max_due = get_current_time() + FUTURE_TIME;

    notification_list_t list = { 0 };

    log_info("General usage: Getting all next unchecked notifications before time\n");

    int r = db_find_all_notifications_unchecked_before_time(db, &list, max_due);
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

// 12y34M56d78h90m
int parse_absolute_time(char * buffer, ssize_t char_count, int64_t * result) {
    datetime_t dt = { 0 };
    uint16_t temp = 0;
    for(int i = 0; i < char_count; i++) {
        if(isdigit(buffer[i])) temp = temp * 10 + (buffer[i] - '0');
        else {
            switch (buffer[i]) {
            case 'y': dt.year = temp; break;
            case 'M': dt.month = temp; break;
            case 'd': dt.day = temp; break;
            case 'h': dt.hour = temp; break;
            case 'm': dt.minute = temp; break;
            default:
                pretty_print("Parsing error: cannot parse format character '%c' in absolute time, only one of the following is allowed: [yMdhm]\n", buffer[i]);
                return 0;
            }
            temp = 0;
        }
    }

    // Parse month
    {
        if(dt.month < 1 || dt.month > 12 ) {
            pretty_print("Range error: month can only be in intervanl [1, 12], got %d\n", dt.month);
            return 0;
        }
    }

    // Parse day
    {
        int days_in_month = datetime_day_in_month(dt.year, dt.month);
        if(dt.day < 1 || dt.day > days_in_month) {
            pretty_print("Range error: day can only be in interval [1, %d]\n, got %d ", days_in_month, dt.day);
            return 0;
        }
    }

    // Parse hour
    {
        if(dt.hour > 23) {
            pretty_print("Range error: hour can only be in interval [0, 23], got %d\n", dt.hour);
            return 0;
        }
    }

    // Parse minute
    {
        if(dt.minute > 59) {
            pretty_print("Range error: minute can only be in interval [0, 59], got %d\n", dt.minute);
            return 0;
        }
    }

    int64_t temp2 = datetime_from(&dt);

    {
        if(temp2 == 0) {
            pretty_print("Other error: conversion failed for datetime string '%s'\n", buffer);
            return 0;
        }
    }

    *result = temp2;

    return 1;
}

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
    pretty_print("- Delta time: +12w34d56h78m\n");
    pretty_print("- Absolute time: 12y34M56d78h90m\n");

    int64_t entered_time = -1;

    while(entered_time == -1) {
        char * buffer = 0;
        size_t buffersize = 0;
        ssize_t char_count = getline(&buffer, &buffersize, stdin);

        // Remove \n
        if(buffer[char_count - 1] = '\n') {
            buffer[char_count - 1] = 0;
            char_count--;
        }

        entered_time = parse_different_time_formats(buffer, char_count);

        // According to the manual of getline, the buffer should be freed by the user
        free(buffer);
    }
}

int64_t parse_different_time_formats(char * buffer, ssize_t char_count) {
    int64_t result = 0;

    // Unix time parsed
    //if(sscanf(buffer, "%ldu", &result) == 1) {
    //    return result;
    //}

    // TODO: add date to the time format
    /*if(sscanf(buffer, "+%dd%dh%dm"))

    if(buffer[char_count - 1] == 'u') {
        // Check and convert UNIX to abs

        // Exclude u and \n
        for(int i = 0; i < char_count - 2; i++) {
            if(buffer[i] > '9' || buffer[i] < '0') {
                pretty_print("The entered time was invalid, try again");
                return -1;
            }
        }

        // Remove the u
        buffer[char_count - 2] = 0;

        char * unix_time = buffer + 1;

        return atol(unix_time);

    } else */
    if(buffer[0] == '+') {
        pretty_print("Not implemented error: delta time is not implemented yet\n");

        return -1;
    } else {
        int64_t absolute_time = 0;
        if(parse_absolute_time(buffer, char_count, &absolute_time) == 0) return -1;

        return absolute_time;
    }
}