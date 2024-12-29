#include <sqlite3.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#include "db.h"
#include "log.h"
#include "sql.h"
#include "fmt.h"

//#define CREATED_FILE "/home/onraj/.tenoti/created"
#define CREATED_FILE "db/created"

//#define FIRST_DUE_FILE "/home/onraj/.tenoti/first_due"
#define FIRST_DUE_FILE "db/first_due"

//#define DB_FILE "/home/onraj/.tenoti/db.sqlite3"
#define DB_FILE "db/test.db"

#define FUTURE_TIME 2 * 60 * 60 // max 2 hrs in advance

/*int64_t get_first_due(sqlite3 * db) {
    log_info("Get first due: Getting first due\n");
    notification_t n = { 0 };
    int r = find_first_next_notification(db, &n);

    if(!n.exists) {
        log_info("Get first due: No first due\n");
        return -1;
    }

    return n.due;
}*/

sqlite3 * init();
static int general_callback(void * void_notification_list, int argc, char ** argv, char ** azColName);

int64_t get_current_time();
int64_t from_now(int64_t t);

void general_usage(sqlite3 * db);

int main(int argc, char ** argv) {
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
    
    /*log_info("Main: Opening DB\n");
    db_open("db/test.db", &db);

    log_info("Main: Initializing DB\n");
    init(db);

    notification_t notification = { 0 };

    log_info("Main: Checking first due notification\n");
    check_first_due(db, &notification);

    log_info("Main: inserting into notifications\n");
    insert_into_notifications(db, "Title", time(NULL), time(NULL) - 60, 0, 0, 0);

    if(find_first_next_notification(db, &notification) == -1) {
        log_error("Main: Error while finding next notification\n");
    }

    if(!notification.exists) {
        log_info("Main: No next notification\n");
        db_close(db);
        return 0;
    }

    char * str = notification_to_str(&notification);

    pretty_print("%s\n", str);

    free(str);

    notification_list_t list = { 0 };
    find_all_overdue_notifications(db, &list);
    free_notification_list(&list);

    db_close(db);

    return 0;*/
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