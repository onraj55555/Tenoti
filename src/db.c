#include "db.h"
#include "log.h"
#include "sql.h"
#include "fmt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK_SQLITE_BIND(statement, error_msg) if(statement != SQLITE_OK) { sqlite3_finalize(stmt); log_error(error_msg); return -1; }

int db_general_query(sqlite3 * db, notification_list_t * list, char * query) {
    sqlite3_stmt * stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        return -1;
    }

    rc = db_exec(db, stmt, list);

    sqlite3_finalize(stmt);

    return rc;
}

int db_general_query_param(sqlite3 * db, notification_list_t * list, char * query, int buffer_size, int64_t param) {
    sqlite3_stmt * stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        -1;
    }

    CHECK_SQLITE_BIND(sqlite3_bind_int64(stmt, 1, param), "DB_genera_query_param: Failed to bind param\n");

    rc = db_exec(db, stmt, list);

    sqlite3_finalize(stmt);

    return rc;
}

int db_find_all_notifications(sqlite3 * db, notification_list_t * list) {
    return db_general_query(db, list, SELECT_ALL_NOTIFICATIONS);
}

int db_find_all_notifications_before_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_BEFORE_TIME, SELECT_ALL_NOTIFICATIONS_BEFORE_TIME_BUFFER, t);
}

int db_find_all_notifications_after_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_AFTER_TIME, SELECT_ALL_NOTIFICATIONS_AFTER_TIME_BUFFER, t);
}

int db_find_all_notifications_unchecked(sqlite3 * db, notification_list_t * list) {
    return db_general_query(db, list, SELECT_ALL_NOTIFICATIONS_UNCHECKED);
}

int db_find_all_notifications_checked(sqlite3 * db, notification_list_t * list) {
    return db_general_query(db, list, SELECT_ALL_NOTIFICATIONS_CHECKED);
}

int db_find_all_notifications_unchecked_before_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_UNCHECKED_BEFORE_TIME, SELECT_ALL_NOTIFICATIONS_UNCHECKED_BEFORE_TIME_BUFFER, t);
}

int db_find_all_notifications_unchecked_after_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_UNCHECKED_AFTER_TIME, SELECT_ALL_NOTIFICATIONS_UNCHECKED_AFTER_TIME_BUFFER, t);
}

int db_find_all_notifications_checked_before_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_CHECKED_BEFORE_TIME, SELECT_ALL_NOTIFICATIONS_CHECKED_BEFORE_TIME_BUFFER, t);
}

int db_find_all_notifications_checked_after_time(sqlite3 * db, notification_list_t * list, int64_t t) {
    return db_general_query_param(db, list, SELECT_ALL_NOTIFICATIONS_CHECKED_AFTER_TIME, SELECT_ALL_NOTIFICATIONS_CHECKED_AFTER_TIME_BUFFER, t);
}

int db_insert_into_notifications(sqlite3 * db, notification_t * notification) {
    sqlite3_stmt * stmt;
    int rc;

    rc = sqlite3_prepare_v2(db, INSERT_INTO_NOTIFICATIONS, -1, &stmt, NULL);

    if(rc != SQLITE_OK) {
        return -1;
    }

    // -1 so it finds the length of the string itself and the string is max 50 bytes long
    // SQLITE_TRANSIENT so that sqlite copies the value and manages its lifetime itself
    CHECK_SQLITE_BIND(sqlite3_bind_text(stmt, 1, notification->title, -1, SQLITE_TRANSIENT), "DB_insert_into_notifications: Failed to bind title\n");
    CHECK_SQLITE_BIND(sqlite3_bind_int64(stmt, 2, notification->created_at), "DB_insert_into_notifications: Failed to bind created_at\n");
    CHECK_SQLITE_BIND(sqlite3_bind_int64(stmt, 3, notification->due), "DB_insert_into_notifications: Failed to bind due\n");
    CHECK_SQLITE_BIND(sqlite3_bind_int(stmt, 4, notification->checked), "DB_insert_into_notifications: Failed to bind checked\n");
    CHECK_SQLITE_BIND(sqlite3_bind_int(stmt, 5, notification->reminder), "DB_insert_into_notifications: Failed to bind reminder\n");
    CHECK_SQLITE_BIND(sqlite3_bind_int64(stmt, 6, notification->details), "DB_insert_into_notifications: Failed to bind details\n");

    rc = db_exec(db, stmt, NULL);

    sqlite3_finalize(stmt);

    return rc;
}

void db_open(const char * db_name, sqlite3 ** db) {
    log_info("DB_open: Opening DB\n");
    int rc = 0;

    rc = sqlite3_open(db_name, db);

    // Error should not happen because the file gets created if it does not exist
    if(rc) {
        log_error("db_open: Can't open db: %s\n", sqlite3_errmsg(*db));
        exit(EXIT_FAILURE);
    }

    log_info("DB_open: DB opened\n");
}

void db_close(sqlite3 * db) {
    sqlite3_close(db);
}

// DOES NOT FINALISE THE PREPARED STATEMENT
int db_exec(sqlite3 * db, sqlite3_stmt * stmt, notification_list_t * list) {
    log_info("DB_exec: Executing\n");

    if(!list) {
        log_debug("DB_exec: No list provided, query does not return data, expecting SQLITE_DONE\n");
        if(sqlite3_step(stmt) != SQLITE_DONE) {
            log_warn("DB_exec: Did not receive done after expecting query which does not return data\n");
            return -1;
        }
        return 1;
    }

    char * zErrMsg = 0;
    int rc;

    int i = 0;

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        notification_t notification = { 0 };

        fill_notification(&notification, sqlite3_column_text(stmt, 0), sqlite3_column_int64(stmt, 1), sqlite3_column_int64(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 4), sqlite3_column_int64(stmt, 5), sqlite3_column_int64(stmt, 6));

        if(append_to_list(list, &notification) == -1) {
            log_error("DB_exec: Failed to append to list\n");
            return -1;
        }
    }

    if(rc != SQLITE_DONE) {
        log_warn("DB_exec: SQL did not return a done\n");
        return -1;
    }

    log_debug("DB_exec: Found %d notifications\n", list->size);

    return 1;
}