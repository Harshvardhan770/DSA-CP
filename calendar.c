#include "calendar.h"

// Initialize the head of the list
Event *head = NULL;

// Global ID counter
static int next_id = 1;

// ========== UTILITY FUNCTIONS ==========

int getNextId()
{
    return next_id++;
}

// Check if a year is a leap year
int isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Get number of days in a month
int getDaysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }
    return days[month - 1];
}

// Validate a date
int isValidDate(int day, int month, int year)
{
    if (year < 1900 || year > 2100)
        return 0;
    if (month < 1 || month > 12)
        return 0;
    if (day < 1 || day > getDaysInMonth(month, year))
        return 0;
    return 1;
}

// Compare two dates: returns -1 if d1 < d2, 0 if equal, 1 if d1 > d2
int compareDates(Date d1, Date d2)
{
    if (d1.year != d2.year)
        return (d1.year < d2.year) ? -1 : 1;
    if (d1.month != d2.month)
        return (d1.month < d2.month) ? -1 : 1;
    if (d1.day != d2.day)
        return (d1.day < d2.day) ? -1 : 1;
    return 0;
}

// Compare two events for sorting (by date, then time, then priority)
int compareEvents(Event *e1, Event *e2)
{
    int date_cmp = compareDates(e1->event_date, e2->event_date);
    if (date_cmp != 0)
        return date_cmp;

    // If same date, compare by time (all-day events come first)
    if (e1->is_all_day && !e2->is_all_day)
        return -1;
    if (!e1->is_all_day && e2->is_all_day)
        return 1;

    if (!e1->is_all_day && !e2->is_all_day)
    {
        if (e1->event_time.hour != e2->event_time.hour)
            return (e1->event_time.hour < e2->event_time.hour) ? -1 : 1;
        if (e1->event_time.minute != e2->event_time.minute)
            return (e1->event_time.minute < e2->event_time.minute) ? -1 : 1;
    }

    // If same date and time, sort by priority
    return (e1->priority < e2->priority) ? -1 : ((e1->priority > e2->priority) ? 1 : 0);
}

// Get current date
void getCurrentDate(Date *date)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    date->day = tm_info->tm_mday;
    date->month = tm_info->tm_mon + 1;
    date->year = tm_info->tm_year + 1900;
}

// Convert string to lowercase
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower(str[i]);
    }
}

// Get priority as string
const char *getPriorityString(Priority p)
{
    switch (p)
    {
    case PRIORITY_HIGH:
        return "HIGH";
    case PRIORITY_MEDIUM:
        return "MEDIUM";
    case PRIORITY_LOW:
        return "LOW";
    default:
        return "UNKNOWN";
    }
}

// Get priority color
const char *getPriorityColor(Priority p)
{
    switch (p)
    {
    case PRIORITY_HIGH:
        return COLOR_RED;
    case PRIORITY_MEDIUM:
        return COLOR_YELLOW;
    case PRIORITY_LOW:
        return COLOR_GREEN;
    default:
        return COLOR_RESET;
    }
}

// Get category as string
const char *getCategoryString(Category c)
{
    switch (c)
    {
    case CATEGORY_WORK:
        return "Work";
    case CATEGORY_PERSONAL:
        return "Personal";
    case CATEGORY_BIRTHDAY:
        return "Birthday";
    case CATEGORY_MEETING:
        return "Meeting";
    case CATEGORY_APPOINTMENT:
        return "Appointment";
    case CATEGORY_REMINDER:
        return "Reminder";
    case CATEGORY_OTHER:
        return "Other";
    default:
        return "Unknown";
    }
}

// Get category color
const char *getCategoryColor(Category c)
{
    switch (c)
    {
    case CATEGORY_WORK:
        return COLOR_BLUE;
    case CATEGORY_PERSONAL:
        return COLOR_MAGENTA;
    case CATEGORY_BIRTHDAY:
        return COLOR_CYAN;
    case CATEGORY_MEETING:
        return COLOR_YELLOW;
    case CATEGORY_APPOINTMENT:
        return COLOR_GREEN;
    case CATEGORY_REMINDER:
        return COLOR_RED;
    case CATEGORY_OTHER:
        return COLOR_RESET;
    default:
        return COLOR_RESET;
    }
}

// ========== INITIALIZATION ==========

void initializeCalendar()
{
    loadEventsFromFile("calendar_data.bin");
}

// ========== EVENT MANAGEMENT ==========

// Insert event in sorted order
void insertEvent(Date date, Time time, const char *desc, Priority priority, Category category, int is_all_day)
{
    Event *newEvent = (Event *)malloc(sizeof(Event));
    if (newEvent == NULL)
    {
        printf("%sError: Could not allocate memory for new event.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    newEvent->id = getNextId();
    newEvent->event_date = date;
    newEvent->event_time = time;
    strncpy(newEvent->description, desc, MAX_DESC_LENGTH - 1);
    newEvent->description[MAX_DESC_LENGTH - 1] = '\0';
    newEvent->priority = priority;
    newEvent->category = category;
    newEvent->is_all_day = is_all_day;
    newEvent->next = NULL;

    // Insert in sorted order
    if (head == NULL || compareEvents(newEvent, head) < 0)
    {
        newEvent->next = head;
        head = newEvent;
    }
    else
    {
        Event *current = head;
        while (current->next != NULL && compareEvents(newEvent, current->next) >= 0)
        {
            current = current->next;
        }
        newEvent->next = current->next;
        current->next = newEvent;
    }

    printf("\n%s✓ Event added successfully! ID: %d%s\n", COLOR_GREEN, newEvent->id, COLOR_RESET);
}

// Delete event by ID
void deleteEvent(int id)
{
    Event *current = head;
    Event *prev = NULL;

    if (current != NULL && current->id == id)
    {
        head = current->next;
        free(current);
        printf("\n%s✓ Event ID %d deleted successfully.%s\n", COLOR_GREEN, id, COLOR_RESET);
        return;
    }

    while (current != NULL && current->id != id)
    {
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        printf("\n%s✗ Error: Event ID %d not found.%s\n", COLOR_RED, id, COLOR_RESET);
        return;
    }

    prev->next = current->next;
    free(current);
    printf("\n%s✓ Event ID %d deleted successfully.%s\n", COLOR_GREEN, id, COLOR_RESET);
}

// Edit event
void editEvent(int id, const char *new_desc, Priority new_priority, Category new_category)
{
    Event *current = head;
    while (current != NULL && current->id != id)
    {
        current = current->next;
    }

    if (current == NULL)
    {
        printf("\n%s✗ Error: Event ID %d not found.%s\n", COLOR_RED, id, COLOR_RESET);
        return;
    }

    if (new_desc != NULL && strlen(new_desc) > 0)
    {
        strncpy(current->description, new_desc, MAX_DESC_LENGTH - 1);
        current->description[MAX_DESC_LENGTH - 1] = '\0';
    }
    current->priority = new_priority;
    current->category = new_category;

    printf("\n%s✓ Event ID %d updated successfully.%s\n", COLOR_GREEN, id, COLOR_RESET);
}

// ========== DISPLAY FUNCTIONS ==========

void displayAllEvents()
{
    Event *current = head;
    if (current == NULL)
    {
        printf("\n%s--- The calendar is empty. ---%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }

    printf("\n%s%s=== ALL SCHEDULED EVENTS ===%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    while (current != NULL)
    {
        printf("\n%sID: %d%s | ", COLOR_BOLD, current->id, COLOR_RESET);
        printf("Date: %02d/%02d/%d | ", current->event_date.day, current->event_date.month, current->event_date.year);

        if (current->is_all_day)
        {
            printf("Time: All Day | ");
        }
        else
        {
            printf("Time: %02d:%02d | ", current->event_time.hour, current->event_time.minute);
        }

        printf("%sPriority: %s%s | ", getPriorityColor(current->priority),
               getPriorityString(current->priority), COLOR_RESET);
        printf("%sCategory: %s%s\n", getCategoryColor(current->category),
               getCategoryString(current->category), COLOR_RESET);
        printf("Description: %s\n", current->description);

        current = current->next;
    }
    printf("\n%s===============================%s\n", COLOR_CYAN, COLOR_RESET);
}

void displayAllEventsSorted()
{
    displayAllEvents(); // Already sorted due to insertion
}

void displayEventsByMonth(int month)
{
    Event *current = head;
    int found = 0;

    printf("\n%s=== EVENTS FOR MONTH %02d ===%s\n", COLOR_CYAN, month, COLOR_RESET);
    while (current != NULL)
    {
        if (current->event_date.month == month)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);

            if (current->is_all_day)
            {
                printf("All Day | ");
            }
            else
            {
                printf("%02d:%02d | ", current->event_time.hour, current->event_time.minute);
            }

            printf("%s%s%s | %s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET,
                   getCategoryColor(current->category), getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events found for month %02d.%s\n", COLOR_YELLOW, month, COLOR_RESET);
    }
    printf("%s==============================%s\n", COLOR_CYAN, COLOR_RESET);
}

void displayEventsByCategory(Category category)
{
    Event *current = head;
    int found = 0;

    printf("\n%s=== EVENTS IN CATEGORY: %s ===%s\n", COLOR_CYAN, getCategoryString(category), COLOR_RESET);
    while (current != NULL)
    {
        if (current->category == category)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);
            printf("%s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events found in this category.%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    printf("%s==============================%s\n", COLOR_CYAN, COLOR_RESET);
}

void displayEventsByPriority(Priority priority)
{
    Event *current = head;
    int found = 0;

    printf("\n%s=== EVENTS WITH PRIORITY: %s ===%s\n", COLOR_CYAN, getPriorityString(priority), COLOR_RESET);
    while (current != NULL)
    {
        if (current->priority == priority)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);
            printf("%s%s%s\n", getCategoryColor(current->category),
                   getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events found with this priority.%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    printf("%s==============================%s\n", COLOR_CYAN, COLOR_RESET);
}

void displayTodaysEvents()
{
    Date today;
    getCurrentDate(&today);
    Event *current = head;
    int found = 0;

    printf("\n%s%s=== TODAY'S EVENTS (%02d/%02d/%d) ===%s\n",
           COLOR_BOLD, COLOR_CYAN, today.day, today.month, today.year, COLOR_RESET);

    while (current != NULL)
    {
        if (compareDates(current->event_date, today) == 0)
        {
            printf("\n%sID: %d%s | ", COLOR_BOLD, current->id, COLOR_RESET);

            if (current->is_all_day)
            {
                printf("All Day | ");
            }
            else
            {
                printf("%02d:%02d | ", current->event_time.hour, current->event_time.minute);
            }

            printf("%s%s%s | %s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET,
                   getCategoryColor(current->category), getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events scheduled for today.%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    printf("%s================================%s\n", COLOR_CYAN, COLOR_RESET);
}

void displayUpcomingEvents(int days)
{
    Date today, end_date;
    getCurrentDate(&today);

    // Calculate end date (simplified - doesn't handle month/year overflow perfectly)
    end_date = today;
    end_date.day += days;

    // Handle month overflow (simplified)
    while (end_date.day > getDaysInMonth(end_date.month, end_date.year))
    {
        end_date.day -= getDaysInMonth(end_date.month, end_date.year);
        end_date.month++;
        if (end_date.month > 12)
        {
            end_date.month = 1;
            end_date.year++;
        }
    }

    Event *current = head;
    int found = 0;

    printf("\n%s=== UPCOMING EVENTS (Next %d days) ===%s\n", COLOR_CYAN, days, COLOR_RESET);

    while (current != NULL)
    {
        if (compareDates(current->event_date, today) >= 0 &&
            compareDates(current->event_date, end_date) <= 0)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);

            if (current->is_all_day)
            {
                printf("All Day | ");
            }
            else
            {
                printf("%02d:%02d | ", current->event_time.hour, current->event_time.minute);
            }

            printf("%s%s%s | %s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET,
                   getCategoryColor(current->category), getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo upcoming events in the next %d days.%s\n", COLOR_YELLOW, days, COLOR_RESET);
    }
    printf("%s====================================%s\n", COLOR_CYAN, COLOR_RESET);
}

// ========== SEARCH FUNCTIONS ==========

void searchEventByKeyword(const char *keyword)
{
    Event *current = head;
    int found = 0;
    char lower_keyword[100], lower_desc[MAX_DESC_LENGTH];

    strncpy(lower_keyword, keyword, 99);
    lower_keyword[99] = '\0';
    toLowerCase(lower_keyword);

    printf("\n%s=== SEARCH RESULTS for '%s' ===%s\n", COLOR_CYAN, keyword, COLOR_RESET);

    while (current != NULL)
    {
        strncpy(lower_desc, current->description, MAX_DESC_LENGTH - 1);
        lower_desc[MAX_DESC_LENGTH - 1] = '\0';
        toLowerCase(lower_desc);

        if (strstr(lower_desc, lower_keyword) != NULL)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);
            printf("%s%s%s | %s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET,
                   getCategoryColor(current->category), getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events found matching '%s'.%s\n", COLOR_YELLOW, keyword, COLOR_RESET);
    }
    printf("%s==============================%s\n", COLOR_CYAN, COLOR_RESET);
}

void searchEventsByDateRange(Date start_date, Date end_date)
{
    Event *current = head;
    int found = 0;

    printf("\n%s=== EVENTS FROM %02d/%02d/%d TO %02d/%02d/%d ===%s\n",
           COLOR_CYAN, start_date.day, start_date.month, start_date.year,
           end_date.day, end_date.month, end_date.year, COLOR_RESET);

    while (current != NULL)
    {
        if (compareDates(current->event_date, start_date) >= 0 &&
            compareDates(current->event_date, end_date) <= 0)
        {
            printf("\n%sID: %d%s | Date: %02d/%02d/%d | ", COLOR_BOLD, current->id, COLOR_RESET,
                   current->event_date.day, current->event_date.month, current->event_date.year);
            printf("%s%s%s | %s%s%s\n", getPriorityColor(current->priority),
                   getPriorityString(current->priority), COLOR_RESET,
                   getCategoryColor(current->category), getCategoryString(current->category), COLOR_RESET);
            printf("Description: %s\n", current->description);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("%sNo events found in this date range.%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    printf("%s===========================================%s\n", COLOR_CYAN, COLOR_RESET);
}

// ========== FILE OPERATIONS ==========

void saveEventsToFile(const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("\n%sError: Could not open file %s for saving.%s\n", COLOR_RED, filename, COLOR_RESET);
        return;
    }

    Event *current = head;
    int count = 0;

    fwrite(&next_id, sizeof(int), 1, fp);

    while (current != NULL)
    {
        fwrite(current, sizeof(Event) - sizeof(Event *), 1, fp);
        current = current->next;
        count++;
    }

    fclose(fp);
    printf("\n%s✓ Successfully saved %d events to %s.%s\n", COLOR_GREEN, count, filename, COLOR_RESET);
}

void loadEventsFromFile(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("%sNote: Data file not found. Starting with empty calendar.%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }

    if (fread(&next_id, sizeof(int), 1, fp) != 1)
    {
        printf("%sError reading ID counter from file.%s\n", COLOR_RED, COLOR_RESET);
        fclose(fp);
        return;
    }

    Event event_buffer;
    Event *tail = NULL;

    while (fread(&event_buffer, sizeof(Event) - sizeof(Event *), 1, fp) == 1)
    {
        Event *newEvent = (Event *)malloc(sizeof(Event));
        if (newEvent == NULL)
            continue;

        *newEvent = event_buffer;
        newEvent->next = NULL;

        if (head == NULL)
        {
            head = newEvent;
            tail = newEvent;
        }
        else
        {
            tail->next = newEvent;
            tail = newEvent;
        }
    }

    fclose(fp);
    printf("%s--- Loaded calendar data successfully. ---%s\n", COLOR_GREEN, COLOR_RESET);
}

void exportToCSV(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("\n%sError: Could not create CSV file.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }

    fprintf(fp, "ID,Date,Time,Priority,Category,Description\n");

    Event *current = head;
    int count = 0;

    while (current != NULL)
    {
        fprintf(fp, "%d,%02d/%02d/%d,", current->id,
                current->event_date.day, current->event_date.month, current->event_date.year);

        if (current->is_all_day)
        {
            fprintf(fp, "All Day,");
        }
        else
        {
            fprintf(fp, "%02d:%02d,", current->event_time.hour, current->event_time.minute);
        }

        fprintf(fp, "%s,%s,\"%s\"\n", getPriorityString(current->priority),
                getCategoryString(current->category), current->description);

        current = current->next;
        count++;
    }

    fclose(fp);
    printf("\n%s✓ Successfully exported %d events to %s%s\n", COLOR_GREEN, count, filename, COLOR_RESET);
}

// ========== STATISTICS ==========

void displayStatistics()
{
    Event *current = head;
    int total = 0, high = 0, medium = 0, low = 0;
    int categories[7] = {0};

    while (current != NULL)
    {
        total++;
        if (current->priority == PRIORITY_HIGH)
            high++;
        else if (current->priority == PRIORITY_MEDIUM)
            medium++;
        else if (current->priority == PRIORITY_LOW)
            low++;

        categories[current->category]++;
        current = current->next;
    }

    printf("\n%s%s=== CALENDAR STATISTICS ===%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("\nTotal Events: %s%d%s\n", COLOR_BOLD, total, COLOR_RESET);
    printf("\n%sPriority Distribution:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  %sHigh:%s %d\n", COLOR_RED, COLOR_RESET, high);
    printf("  %sMedium:%s %d\n", COLOR_YELLOW, COLOR_RESET, medium);
    printf("  %sLow:%s %d\n", COLOR_GREEN, COLOR_RESET, low);

    printf("\n%sCategory Distribution:%s\n", COLOR_BOLD, COLOR_RESET);
    for (int i = 0; i < 7; i++)
    {
        if (categories[i] > 0)
        {
            printf("  %s%s:%s %d\n", getCategoryColor(i), getCategoryString(i), COLOR_RESET, categories[i]);
        }
    }
    printf("%s=========================%s\n", COLOR_CYAN, COLOR_RESET);
}

// ========== CLEANUP ==========

void freeCalendar()
{
    Event *current = head;
    Event *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}