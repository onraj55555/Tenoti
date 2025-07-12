#ifndef MAIN_H_
#define MAIN_H_

#include <sqlite3.h>
#include <stdint.h>

#define CALL_SUCCESS 1
#define CALL_FAIL 0

//#define CREATED_FILE "/home/onraj/.tenoti/created"
#define CREATED_FILE "db/created"

//#define FIRST_DUE_FILE "/home/onraj/.tenoti/first_due"
#define FIRST_DUE_FILE "db/first_due"

//#define DB_FILE "/home/onraj/.tenoti/db.sqlite3"
#define DB_FILE "db/test.db"

#define FUTURE_TIME 2 * 60 * 60 // max 2 hrs in advance

/**
 * @brief Returns the current time in UNIX time format
 * 
 * @return An int64_t type representing the current UNIX time
 */
int64_t get_current_time();

/**
 * @brief Returns the seconds between the given UNIX time and the current UNIX time, calls get_current_time()
 * 
 * @return An int64_t representing the seconds passed between the given time and the current time
 */
int64_t from_now(int64_t t);

void parse_args(int argc, char ** argv);


/**
 * @brief Functionality for the add command specifief with "executable a"
 */
void command_add();

void command_general_usage();

#endif