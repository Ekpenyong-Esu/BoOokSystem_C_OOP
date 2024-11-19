#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <syslog.h>
#include <time.h>

#define MAX_TITLE_LENGTH 100
#define MAX_AUTHOR_LENGTH 100
#define MAX_ISBN_LENGTH 20
#define MAX_NAME_LENGTH 50
#define MAX_EMAIL_LENGTH 100
#define MAX_BORROWED_BOOKS 5
#define INITIAL_CAPACITY 10

typedef struct
{
    int ident;
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char isbn[MAX_ISBN_LENGTH];
    int is_available;
    time_t added_date;
} Book;

typedef struct
{
    int ident;
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int borrowed_books[MAX_BORROWED_BOOKS];
    int num_borrowed_books;
} Member;

typedef struct
{
    Book *books;
    int num_books;
    int capacity_books;
    Member *members;
    int num_members;
    int capacity_members;
} Library;

#endif
