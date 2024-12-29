# Tenito
Terminal-based notification system using SQLite

# DB structure
## Notifications
This table consists out of fixed-size records containing only a limited amount of information about a notification

### Record structure
| created_at(in64) | title(string @ 50 chars?) | due(int64) | checked(bool) | details(int64) |

- created_at: the datetime on when the notification is created
- title: title of the notification, size may change because of requirements/performance
- due: the datetime on when the notification should go off when a new terminal window is opened
- checked: determines if the notification should be entirely ignored
- details: the primary key of the entry of the details table corresponding to this notification (0 means that there is no info)

NOTE: dates are stored in Unix-time format: https://en.wikipedia.org/wiki/Unix_time

### Tricks
In case you need to set multiple reminders for the same notification, the same notification gets stored in the db multiple times, because every record has a unique row id this is no problem and they all point to the same entry in the details table

## Details
Details about the notification like the title and description

### Record structure
| title(char(50)) | description(varchar) |

## Interaction
rowid @ details == details @ notifications

# Other files
## Created
A file stored together with the db file that indicates if the db is already created, for fast launch

## FirstDue
A file containing the first next notification since the last expired notification, so no db queries are needed when there is no possible notification