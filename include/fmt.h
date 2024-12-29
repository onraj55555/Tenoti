#ifndef FMT_H_
#define FMT_H_

#include "schemas.h"
#include <stdlib.h>

void pretty_print(const char * fmt, ...);

void unix_to_datetime(int64_t unix_time, char *buffer, size_t buffer_size);

char * notification_to_str(notification_t * notification);

void atonotification(notification_t * notification, char ** argv);

#endif