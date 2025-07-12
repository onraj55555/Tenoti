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

#include "main.h"
#include "db.h"
#include "log.h"
#include "sql.h"
#include "fmt.h"
#include "datetime.h"


//#define CREATED_FILE "/home/onraj/.tenoti/created"
#define CREATED_FILE "db/created"

//#define FIRST_DUE_FILE "/home/onraj/.tenoti/first_due"
#define FIRST_DUE_FILE "db/first_due"

#define FUTURE_TIME 2 * 60 * 60 // max 2 hrs in advance

// Used timer, global in case testing is needed
time_t * TIMER = NULL;

int64_t get_current_time();
int64_t from_now(int64_t t);

void general_usage();
void parse_args(int argc, char ** argv);

void add_notification(sqlite3 * db);

int64_t get_current_time() {
    return time(TIMER);
}

void parse_args(int argc, char ** argv) {
    if(argc == 0) {
        log_info("parse_args: General usage\n");
        command_general_usage();
    }
    // Add notification
    if(strcmp(argv[0], "add") == 0) {
        log_info("parse_args: Add argument found\n");
        command_add();
    }
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

    parse_args(argc - 1, argv + 1);

    db_close();

    return 0;
}

int64_t from_now(int64_t t) {
    return t - get_current_time();
}
