#include "main.h"
#include "schemas.h"
#include "fmt.h"
#include "log.h"

void command_general_usage() {
    // Get all notifications which might be due
    // A.k.a. check which aren't checked yet and their due <= current_time + FUTURE_TIME

    log_info("General usage: Start\n");

    int64_t max_due = get_current_time() + FUTURE_TIME;

    notification_list_t list = { 0 };

    log_info("General usage: Getting all next unchecked notifications before time\n");

    int r = db_find_all_notifications_unchecked_before_time(&list, max_due);
    //int r = db_find_all_notifications(db, &list);
    log_info("General usage: Done getting them\n");

    if(r == -1) {
        log_error("General usage: An error occured while getting all due notifications\n");

        free_list(&list);

        return;
    }

    notification_link_t * link = list.notifications_link;
    while(link) {
        char * buffer = notification_to_str(&link->notification);
        pretty_print("%s\n", buffer);
        link = link->next;
    }

    free_list(&list);
}