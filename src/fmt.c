#include "fmt.h"
#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void atonotification(notification_t * notification, char ** argv) {
    log_info("Atonotification: String to notification\n");
    strncpy(notification->title, argv[0], 50);
    notification->created_at = atol(argv[1]);
    notification->due = atol(argv[2]);
    notification->checked = argv[3][0] - '0';
    notification->reminder = argv[4][0] - '0';

    if(argv[5] == NULL) notification->details = -1;
    else notification->details = atol(argv[5]);

    notification->exists = 1;
}

void pretty_print(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "tenito > ");
    vfprintf(stderr, fmt, args);

    va_end(args);
}

void unix_to_datetime(int64_t unix_time, char *buffer, size_t buffer_size) {
    struct tm *tm_info;
    time_t time = (time_t)unix_time;

    tm_info = localtime(&time);
    strftime(buffer, buffer_size, "%d-%m-%Y-%H-%M-%S", tm_info);
}

char * notification_to_str(notification_t * notification) {
    char created_at_buffer[20] = { 0 };
    char due_buffer[20] = { 0 };

    unix_to_datetime(notification->created_at, created_at_buffer, sizeof(created_at_buffer));
    unix_to_datetime(notification->due, due_buffer, sizeof(due_buffer));

    char * fmt = "{title: %s, created_at: %s, due: %s, checked: %d, reminder: %d, details: %ld}";

    int buffer_size = 165;

    char * buffer = malloc(buffer_size);

    if(!buffer) return NULL;

    snprintf(buffer, buffer_size, fmt, notification->title, created_at_buffer, due_buffer, notification->checked, notification->reminder, notification->details);

    return buffer;
}