#ifndef LOG_H_
#define LOG_H_

// Error: 1
// Warn: 2
// Debug: 3
// Info: 4

#include <stdio.h>
#include "db.h"

void panic(const char * fmt, ...);

#define TODO(msg) \
    panic("TODO at %s:%d — %s\n", __FILE__, __LINE__, msg)

#define LOGLEVEL 4

void log_error(const char *fmt, ...);

void log_warn(const char * fmt, ...);

void log_debug(const char * fmt, ...);

void log_info(const char * fmt, ...);

#endif