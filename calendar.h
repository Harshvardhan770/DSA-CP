#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// --- Constants ---
#define MAX_DESC_LENGTH 100
#define MAX_CATEGORY_LENGTH 30

// --- ANSI Color Codes ---
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_BOLD    "\x1b[1m"

// --- Enumerations ---

// Priority levels for events
typedef enum {
    PRIORITY_LOW = 3,
    PRIORITY_MEDIUM = 2,
    PRIORITY_HIGH = 1
} Priority;

// Event categories
typedef enum {
    CATEGORY_WORK,
    CATEGORY_PERSONAL,
    CATEGORY_BIRTHDAY,
    CATEGORY_MEETING,
    CATEGORY_APPOINTMENT,
    CATEGORY_REMINDER,
    CATEGORY_OTHER
} Category;

// --- Data Structures ---

// Structure to hold date information
typedef struct Date {
    int day;
    int month;
    int year;
} Date;

// Structure to hold time information
typedef struct Time {
    int hour;   // 0-23
    int minute; // 0-59
} Time;

// Structure for a single calendar event
typedef struct Event {
    int id;
    Date event_date;
    Time event_time;
    char description[MAX_DESC_LENGTH];
    Priority priority;
    Category category;
    int is_all_day; // 1 if all-day event, 0 if timed
    struct Event *next;
} Event;

// --- Global Variable ---
extern Event *head;

// --- Function Prototypes ---

// Initialization & Cleanup
void initializeCalendar();
void freeCalendar();
int getNextId();

// Event Management
void insertEvent(Date date, Time time, const char *desc, Priority priority, Category category, int is_all_day);
void deleteEvent(int id);
void editEvent(int id, const char *new_desc, Priority new_priority, Category new_category);

// Display Functions
void displayAllEvents();
void displayAllEventsSorted();
void displayEventsByMonth(int month);
void displayEventsByCategory(Category category);
void displayEventsByPriority(Priority priority);
void displayTodaysEvents();
void displayUpcomingEvents(int days);

// Search Functions
void searchEventByKeyword(const char *keyword);
void searchEventsByDateRange(Date start_date, Date end_date);

// File Operations
void saveEventsToFile(const char *filename);
void loadEventsFromFile(const char *filename);
void exportToCSV(const char *filename);

// Utility Functions
int isValidDate(int day, int month, int year);
int isLeapYear(int year);
int getDaysInMonth(int month, int year);
int compareDates(Date d1, Date d2);
int compareEvents(Event *e1, Event *e2);
void getCurrentDate(Date *date);
const char* getPriorityString(Priority p);
const char* getPriorityColor(Priority p);
const char* getCategoryString(Category c);
const char* getCategoryColor(Category c);
void toLowerCase(char *str);

// Statistics
void displayStatistics();

#endif