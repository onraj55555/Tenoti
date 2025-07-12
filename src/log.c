#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void generic_log(const char * prefix, const char * fmt, va_list args) {
    fprintf(stderr, "%s ", prefix);
    vfprintf(stderr, fmt, args);
    //fprintf(stderr, "\n");
}

void log_error(const char * fmt, ...) {
    #if LOGLEVEL >= 1
    va_list args;
    va_start(args, fmt);

    generic_log("[ERROR]", fmt, args);

    va_end(args);
    #endif
}

void log_warn(const char * fmt, ...) {
    #if LOGLEVEL >= 2
    va_list args;
    va_start(args, fmt);

    generic_log("[WARN]", fmt, args);

    va_end(args);
    #endif
}

void log_debug(const char * fmt, ...) {
    #if LOGLEVEL >= 3
    va_list args;
    va_start(args, fmt);

    generic_log("[DEBUG]", fmt, args);

    va_end(args);
    #endif
}

void log_info(const char * fmt, ...) {
    #if LOGLEVEL >= 4
    va_list args;
    va_start(args, fmt);

    generic_log("[INFO]", fmt, args);

    va_end(args);
    #endif
}

void panic(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    generic_log("[PANIC]", fmt, args);

    va_end(args);

    db_close();
    exit(EXIT_FAILURE);
}