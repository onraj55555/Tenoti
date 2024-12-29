#ifndef LOG_H_
#define LOG_H_

// Error: 1
// Warn: 2
// Debug: 3
// Info: 4

#define LOGLEVEL 4

void log_error(const char *fmt, ...);

void log_warn(const char * fmt, ...);

void log_debug(const char * fmt, ...);

void log_info(const char * fmt, ...);

#endif