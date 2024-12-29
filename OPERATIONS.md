# ./tenoti
## first_due file exists
### Overdue
1) Search DB for all overdue notifications
2) Display all overdue notifications
3) Mark all reminders as completed
4) Ask user for what notifications that should be marked as completed
5) Write first next UNIX time of an uncompleted notification (can be in the past) to first_due file

### Not overdue
1) Do nothing

## first_due file does not exist
### DB contains uncompleted notifications
1) Make file with UNIX time of first next due notification

### DB does not contain uncompleted notifications
1) Do nothing