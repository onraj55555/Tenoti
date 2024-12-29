#include "schemas.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

void fill_notification(notification_t * notification, char * title, int64_t created_at, int64_t due, int8_t checked, int8_t reminder, int64_t details, int64_t rowid) {
    strncpy(notification->title, title, 50);
    notification->created_at = created_at;
    notification->due = due;
    notification->checked = checked;
    notification->reminder = reminder;
    notification->details = details;
    notification->rowid = rowid;
    notification->exists = 1;
}

int append_to_list(notification_list_t * list, notification_t * notification) {
    if(!list->notifications_link) {
        list->notifications_link = malloc(sizeof(notification_link_t));

        if(!list->notifications_link) {
            log_error("Append to list: Failed to allocate first link\nn");
            return -1;
        }

        memcpy(&list->notifications_link->notification, notification, sizeof(notification_t));
        list->notifications_link->next = 0;
        list->size = 1;
        return 1;
    }

    notification_link_t * current = list->notifications_link;

    while(current->next) {
        current = current->next;
    }

    current->next = malloc(sizeof(notification_link_t));

    if(!current->next) {
        log_error("Append to list: Failed to allocate link\nn");
        return -1;
    }

    memcpy(&current->next->notification, notification, sizeof(notification_t));
    current->next->next = 0;
    list->size++;

    return 1;
}

void free_list(notification_list_t * list) {
    notification_link_t * current = list->notifications_link;

    while(current) {
        notification_link_t * t = current->next;
        free(current);
        current = t;
    }
}