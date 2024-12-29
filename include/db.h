#ifndef DB_H_
#define DB_H_

#include <sqlite3.h>
#include "schemas.h"

// Exits when DB cannot be opened
void db_open(const char * db_name, sqlite3 ** db);
void db_close(sqlite3 * db);

int db_exec(sqlite3 * db, sqlite3_stmt * stmt, notification_list_t * callback_arg);

int db_find_all_notifications(sqlite3 * db, notification_list_t * list);
int db_find_all_notifications_before_time(sqlite3 * db, notification_list_t * list, int64_t t);
int db_find_all_notifications_after_time(sqlite3 * db, notification_list_t * list, int64_t t);
int db_find_all_notifications_unchecked(sqlite3 * db, notification_list_t * list);
int db_find_all_notifications_checked(sqlite3 * db, notification_list_t * list);
int db_find_all_notifications_unchecked_before_time(sqlite3 * db, notification_list_t * list, int64_t t);
int db_find_all_notifications_unchecked_after_time(sqlite3 * db, notification_list_t * list, int64_t t);
int db_find_all_notifications_checked_before_time(sqlite3 * db, notification_list_t * list, int64_t t);
int db_find_all_notifications_checked_after_time(sqlite3 * db, notification_list_t * list, int64_t t);

int db_insert_into_notifications(sqlite3 * db, notification_t * notification);

#endif