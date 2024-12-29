#ifndef SCHEMAS_H_
#define SCHEMAS_H_

#include <stdint.h>

typedef struct notification_t notification_t;

struct notification_t {
    int8_t exists;
    int8_t checked;
    int8_t reminder;
    char title[51];
    int64_t created_at;
    int64_t due;
    int64_t details;
    int64_t rowid;
};

void fill_notification(notification_t * notification, char * title, int64_t created_at, int64_t due, int8_t checked, int8_t reminder, int64_t details, int64_t rowid);

typedef struct notification_link_t notification_link_t;

struct notification_link_t {
    notification_t notification;
    notification_link_t * next;
};

typedef struct notification_list_t notification_list_t;

// TODO: make more efficient by storing a pointer to the tail
struct notification_list_t {
    notification_link_t * notifications_link;
    int size;
};

int append_to_list(notification_list_t * list, notification_t * notification);
void free_list(notification_list_t * list);
#endif