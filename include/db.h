#ifndef DB_H_
#define DB_H_

#include <sqlite3.h>
#include "schemas.h"

extern sqlite3 * db;

// Exits when DB cannot be opened
void db_init();
void db_open(const char * db_name, sqlite3 ** db);
void db_close();

int db_exec(sqlite3_stmt * stmt, notification_list_t * callback_arg);

int db_find_all_notifications(notification_list_t * list);
int db_find_all_notifications_before_time(notification_list_t * list, int64_t t);
int db_find_all_notifications_after_time(notification_list_t * list, int64_t t);
int db_find_all_notifications_unchecked(notification_list_t * list);
int db_find_all_notifications_checked(notification_list_t * list);
int db_find_all_notifications_unchecked_before_time(notification_list_t * list, int64_t t);
int db_find_all_notifications_unchecked_after_time(notification_list_t * list, int64_t t);
int db_find_all_notifications_checked_before_time(notification_list_t * list, int64_t t);
int db_find_all_notifications_checked_after_time(notification_list_t * list, int64_t t);

int db_insert_into_notifications(notification_t * notification);

#endif
