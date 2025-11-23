#include "calendar.h"

// Function prototypes
void displayMenu();
void handleAddEvent();
void handleViewByMonth();
void handleDeleteEvent();
void handleSearchEvent();
void handleEditEvent();
void handleViewByCategory();
void handleViewByPriority();
void handleSearchByDateRange();
void handleExportCSV();
void clearInputBuffer();
int getIntInput(const char *prompt, int min, int max);
Priority getPriorityInput();
Category getCategoryInput();

int main() {
    initializeCalendar();

    int choice;
    const char *data_file = "calendar_data.bin";

    // Display today's events on startup
    printf("\n%s%s=============================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("%s%s   WELCOME TO CALENDAR MANAGER PRO 2.0    %s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("%s%s=============================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    
    displayTodaysEvents();

    do {
        displayMenu();
        printf("\n%sEnter your choice:%s ", COLOR_BOLD, COLOR_RESET);
        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            choice = -1;
        }

        switch (choice) {
            case 1:
                handleAddEvent();
                break;
            case 2:
                displayAllEvents();
                break;
            case 3:
                displayTodaysEvents();
                break;
            case 4:
                handleViewByMonth();
                break;
            case 5:
                handleViewByCategory();
                break;
            case 6:
                handleViewByPriority();
                break;
            case 7: {
                int days = getIntInput("Enter number of days to look ahead (1-365)", 1, 365);
                displayUpcomingEvents(days);
                break;
            }
            case 8:
                handleSearchEvent();
                break;
            case 9:
                handleSearchByDateRange();
                break;
            case 10:
                handleEditEvent();
                break;
            case 11:
                handleDeleteEvent();
                break;
            case 12:
                displayStatistics();
                break;
            case 13:
                handleExportCSV();
                break;
            case 14:
                saveEventsToFile(data_file);
                freeCalendar();
                printf("\n%s%s[OK] Exiting application. Goodbye!%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
                break;
            default:
                printf("\n%s[ERROR] Invalid choice. Please try again.%s\n", COLOR_RED, COLOR_RESET);
        }
        
        if (choice != 14) {
            printf("\n%sPress ENTER to continue...%s", COLOR_YELLOW, COLOR_RESET);
            clearInputBuffer();
            getchar();
        }
    } while (choice != 14);

    return 0;
}

void displayMenu() {
    printf("\n%s%s=============================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("%s%s      CONSOLE CALENDAR MANAGER PRO         %s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("%s%s=============================================%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    printf("\n%s[*] Event Management:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  1.  Add New Event\n");
    printf("  2.  View All Events\n");
    printf("  3.  View Today's Events\n");
    printf("\n%s[*] Filter & Search:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  4.  View Events by Month\n");
    printf("  5.  View Events by Category\n");
    printf("  6.  View Events by Priority\n");
    printf("  7.  View Upcoming Events\n");
    printf("  8.  Search by Keyword\n");
    printf("  9.  Search by Date Range\n");
    printf("\n%s[*] Modify & Export:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  10. Edit Event\n");
    printf("  11. Delete Event\n");
    printf("  12. View Statistics\n");
    printf("  13. Export to CSV\n");
    printf("\n%s[*] Exit:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  14. Save & Exit\n");
    printf("%s---------------------------------------------%s\n", COLOR_CYAN, COLOR_RESET);
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getIntInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s (%d-%d): ", prompt, min, max);
        if (scanf("%d", &value) == 1 && value >= min && value <= max) {
            return value;
        }
        printf("%sInvalid input. Try again.%s\n", COLOR_RED, COLOR_RESET);
        clearInputBuffer();
    }
}

Priority getPriorityInput() {
    printf("\n%sSelect Priority:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  %s1. HIGH%s\n", COLOR_RED, COLOR_RESET);
    printf("  %s2. MEDIUM%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  %s3. LOW%s\n", COLOR_GREEN, COLOR_RESET);
    
    int choice = getIntInput("Enter priority", 1, 3);
    
    switch(choice) {
        case 1: return PRIORITY_HIGH;
        case 2: return PRIORITY_MEDIUM;
        case 3: return PRIORITY_LOW;
        default: return PRIORITY_MEDIUM;
    }
}

Category getCategoryInput() {
    printf("\n%sSelect Category:%s\n", COLOR_BOLD, COLOR_RESET);
    printf("  1. Work\n");
    printf("  2. Personal\n");
    printf("  3. Birthday\n");
    printf("  4. Meeting\n");
    printf("  5. Appointment\n");
    printf("  6. Reminder\n");
    printf("  7. Other\n");
    
    int choice = getIntInput("Enter category", 1, 7);
    return (Category)(choice - 1);
}

void handleAddEvent() {
    Date d;
    Time t;
    char desc[MAX_DESC_LENGTH];
    int is_all_day;
    
    printf("\n%s%s=== ADD NEW EVENT ===%s\n", COLOR_BOLD, COLOR_CYAN, COLOR_RESET);
    
    // Get date with validation
    while (1) {
        d.day = getIntInput("Enter Day", 1, 31);
        d.month = getIntInput("Enter Month", 1, 12);
        d.year = getIntInput("Enter Year", 1900, 2100);
        
        if (isValidDate(d.day, d.month, d.year)) {
            break;
        }
        printf("%s[ERROR] Invalid date entered. Event not added.%s\n", COLOR_RED, COLOR_RESET);
    }
    
    // Check if all-day event
    printf("\nIs this an all-day event? (1=Yes, 0=No): ");
    scanf("%d", &is_all_day);
    
    if (!is_all_day) {
        t.hour = getIntInput("Enter Hour (24-hour format)", 0, 23);
        t.minute = getIntInput("Enter Minute", 0, 59);
    } else {
        t.hour = 0;
        t.minute = 0;
    }
    
    // Get priority
    Priority priority = getPriorityInput();
    
    // Get category
    Category category = getCategoryInput();
    
    // Get description
    clearInputBuffer();
    printf("\n%sEnter Event Description (max %d chars):%s ", COLOR_BOLD, MAX_DESC_LENGTH - 1, COLOR_RESET);
    fgets(desc, sizeof(desc), stdin);
    desc[strcspn(desc, "\n")] = 0;
    
    if (strlen(desc) == 0) {
        printf("%s[ERROR] Description cannot be empty. Event not added.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    
    insertEvent(d, t, desc, priority, category, is_all_day);
}

void handleViewByMonth() {
    printf("\n%s=== VIEW EVENTS BY MONTH ===%s\n", COLOR_CYAN, COLOR_RESET);
    int month = getIntInput("Enter Month", 1, 12);
    displayEventsByMonth(month);
}

void handleViewByCategory() {
    printf("\n%s=== VIEW EVENTS BY CATEGORY ===%s\n", COLOR_CYAN, COLOR_RESET);
    Category category = getCategoryInput();
    displayEventsByCategory(category);
}

void handleViewByPriority() {
    printf("\n%s=== VIEW EVENTS BY PRIORITY ===%s\n", COLOR_CYAN, COLOR_RESET);
    Priority priority = getPriorityInput();
    displayEventsByPriority(priority);
}

void handleDeleteEvent() {
    printf("\n%s=== DELETE EVENT ===%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Show all events first
    displayAllEvents();
    
    if (head == NULL) return;
    
    int id = getIntInput("\nEnter Event ID to delete", 1, 10000);
    
    printf("\n%sAre you sure you want to delete event ID %d? (1=Yes, 0=No):%s ", 
           COLOR_YELLOW, id, COLOR_RESET);
    int confirm;
    scanf("%d", &confirm);
    
    if (confirm == 1) {
        deleteEvent(id);
    } else {
        printf("%s[ERROR] Deletion cancelled.%s\n", COLOR_YELLOW, COLOR_RESET);
    }
}

void handleSearchEvent() {
    char keyword[50];
    printf("\n%s=== SEARCH EVENT BY KEYWORD ===%s\n", COLOR_CYAN, COLOR_RESET);
    clearInputBuffer();
    printf("Enter keyword to search: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = 0;
    
    if (strlen(keyword) == 0) {
        printf("%s[ERROR] Keyword cannot be empty.%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    
    searchEventByKeyword(keyword);
}

void handleSearchByDateRange() {
    Date start_date, end_date;
    
    printf("\n%s=== SEARCH BY DATE RANGE ===%s\n", COLOR_CYAN, COLOR_RESET);
    
    printf("\n%sStart Date:%s\n", COLOR_BOLD, COLOR_RESET);
    while (1) {
        start_date.day = getIntInput("Enter Day", 1, 31);
        start_date.month = getIntInput("Enter Month", 1, 12);
        start_date.year = getIntInput("Enter Year", 1900, 2100);
        
        if (isValidDate(start_date.day, start_date.month, start_date.year)) {
            break;
        }
        printf("%s✗ Invalid date. Try again.%s\n", COLOR_RED, COLOR_RESET);
    }
    
    printf("\n%sEnd Date:%s\n", COLOR_BOLD, COLOR_RESET);
    while (1) {
        end_date.day = getIntInput("Enter Day", 1, 31);
        end_date.month = getIntInput("Enter Month", 1, 12);
        end_date.year = getIntInput("Enter Year", 1900, 2100);
        
        if (isValidDate(end_date.day, end_date.month, end_date.year)) {
            if (compareDates(start_date, end_date) <= 0) {
                break;
            }
            printf("%s[ERROR] End date must be after or equal to start date.%s\n", COLOR_RED, COLOR_RESET);
        } else {
            printf("%s[ERROR] Invalid date. Try again.%s\n", COLOR_RED, COLOR_RESET);
        }
    }
    
    searchEventsByDateRange(start_date, end_date);
}

void handleEditEvent() {
    printf("\n%s=== EDIT EVENT ===%s\n", COLOR_CYAN, COLOR_RESET);
    
    // Show all events first
    displayAllEvents();
    
    if (head == NULL) return;
    
    int id = getIntInput("\nEnter Event ID to edit", 1, 10000);
    
    // Find event first to show current values
    Event *current = head;
    while (current != NULL && current->id != id) {
        current = current->next;
    }
    
    if (current == NULL) {
        printf("%s[ERROR] Event ID %d not found.%s\n", COLOR_RED, id, COLOR_RESET);
        return;
    }
    
    printf("\n%sCurrent Description:%s %s\n", COLOR_BOLD, COLOR_RESET, current->description);
    printf("%sCurrent Priority:%s %s\n", COLOR_BOLD, COLOR_RESET, getPriorityString(current->priority));
    printf("%sCurrent Category:%s %s\n", COLOR_BOLD, COLOR_RESET, getCategoryString(current->category));
    
    char new_desc[MAX_DESC_LENGTH];
    clearInputBuffer();
    printf("\n%sEnter NEW description (or press ENTER to keep current):%s ", COLOR_BOLD, COLOR_RESET);
    fgets(new_desc, sizeof(new_desc), stdin);
    new_desc[strcspn(new_desc, "\n")] = 0;
    
    Priority new_priority = getPriorityInput();
    Category new_category = getCategoryInput();
    
    if (strlen(new_desc) == 0) {
        editEvent(id, current->description, new_priority, new_category);
    } else {
        editEvent(id, new_desc, new_priority, new_category);
    }
}

void handleExportCSV() {
    char filename[100];
    printf("\n%s=== EXPORT TO CSV ===%s\n", COLOR_CYAN, COLOR_RESET);
    clearInputBuffer();
    printf("Enter filename (e.g., events.csv): ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;
    
    if (strlen(filename) == 0) {
        strcpy(filename, "calendar_export.csv");
    }
    
    exportToCSV(filename);
}