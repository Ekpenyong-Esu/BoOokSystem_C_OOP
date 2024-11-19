#include "book_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int next_book_id = 1;

void init_book(Book *book,
               const char *title,
               const char *author,
               const char *isbn)
{
    if (!book)
    {
        fprintf(stderr, "Book pointer is NULL\n");
        syslog(LOG_ERR, "Book pointer is NULL\n");
        return;
    }

    book->ident = next_book_id++;
    strncpy(book->title, title ? title : "", MAX_TITLE_LENGTH - 1);
    strncpy(book->author, author ? author : "", MAX_AUTHOR_LENGTH - 1);
    strncpy(book->isbn, isbn ? isbn : "", MAX_ISBN_LENGTH - 1);

    book->title[MAX_TITLE_LENGTH - 1] = '\0';
    book->author[MAX_AUTHOR_LENGTH - 1] = '\0';
    book->isbn[MAX_ISBN_LENGTH - 1] = '\0';

    book->is_available = 1;
    book->added_date = time(NULL);
    syslog(LOG_INFO,
           "Created book with ID: %d, Title: %s, Author: %s, ISBN: %s\n",
           book->ident,
           book->title,
           book->author,
           book->isbn);
}

void deinit_book(Book *book)
{
    // Currently no dynamic memory to free, but this can be expanded in the future
    if (!book)
    {
        fprintf(stderr, "Book pointer is NULL\n");
        syslog(LOG_ERR, "Book pointer is NULL\n");
        return;
    }
    // Placeholder for future cleanup if needed
}

Book *create_book(const char *title, const char *author, const char *isbn)
{
    Book *book = (Book *)malloc(sizeof(Book));
    if (!book)
    {
        fprintf(stderr, "Memory allocation failed for book\n");
        return NULL;
    }

    init_book(book, title, author, isbn);
    return book;
    syslog(LOG_INFO,
           "Created book with Title: %s, Author: %s, ISBN: %s\n",
           title,
           author,
           isbn);
}

void delete_book(Book *book)
{
    if (!book)
    {
        fprintf(stderr, "Book pointer is NULL\n");
        syslog(LOG_ERR, "Book pointer is NULL\n");
        return;
    }
    deinit_book(book);
    free(book);
    syslog(LOG_INFO, "Deleted book\n");
}

void print_book(const Book *book)
{
    if (!book)
    {
        fprintf(stderr, "Book pointer is NULL\n");
        return;
    }

    printf("-----------------\n");
    printf("Book ID: %d\n", book->ident);
    printf("Title: %s\n", book->title);
    printf("Author: %s\n", book->author);
    printf("ISBN: %s\n", book->isbn);
    printf("Status: %s\n", book->is_available ? "Available" : "Borrowed");
    printf("Added on: %s", ctime(&book->added_date));
    printf("-----------------\n");
    syslog(LOG_INFO,
           "Printed book with ID: %d, Title: %s, Author: %s, ISBN: %s\n",
           book->ident,
           book->title,
           book->author,
           book->isbn);
}

int add_book_to_library(Library *library,
                        const char *title,
                        const char *author,
                        const char *isbn)
{
    if (!library || !title || !author || !isbn)
    {
        fprintf(stderr, "Invalid parameters for adding a book\n");
        syslog(LOG_ERR, "Invalid parameters for adding a book\n");
        return 0;
    }

    if (library->num_books >= library->capacity_books)
    {
        int new_capacity = library->capacity_books * 2;
        Book *new_books =
            realloc(library->books, (size_t)new_capacity * sizeof(Book));

        if (!new_books)
        {
            fprintf(stderr,
                    "Memory allocation failed while resizing library\n");
            return 0;
        }

        library->books = new_books;
        library->capacity_books = new_capacity;
    }

    init_book(&library->books[library->num_books], title, author, isbn);
    library->num_books++;
    syslog(LOG_INFO,
           "Added book to library with Title: %s, Author: %s, ISBN: %s\n",
           title,
           author,
           isbn);
    return 1;
}

Book *find_book_by_id(Library *library, int ident)
{
    if (!library)
    {
        fprintf(stderr, "Library pointer is NULL\n");
        syslog(LOG_ERR, "Library pointer is NULL\n");
        return NULL;
    }

    for (int i = 0; i < library->num_books; i++)
    {
        if (library->books[i].ident == ident)
        {
            syslog(LOG_INFO, "Found book with ID: %d\n", ident);
            return &library->books[i];
        }
    }
    return NULL;
}

void remove_book_from_library(Library *library, int ident)
{
    if (!library)
    {
        fprintf(stderr, "Library pointer is NULL\n");
        syslog(LOG_ERR, "Library pointer is NULL\n");
        return;
    }

    int found_index = -1;
    for (int i = 0; i < library->num_books; i++)
    {
        if (library->books[i].ident == ident)
        {
            found_index = i;
            syslog(LOG_INFO, "Removed book with ID: %d\n", ident);
            break;
        }
    }

    if (found_index != -1)
    {
        delete_book(&library->books[found_index]);

        for (int i = found_index; i < library->num_books - 1; i++)
        {
            library->books[i] = library->books[i + 1];
        }
        library->num_books--;
    }
}

void list_all_books(const Library *library)
{
    if (!library)
    {
        fprintf(stderr, "Library pointer is NULL\n");
        syslog(LOG_ERR, "Library pointer is NULL\n");
        return;
    }

    printf("\nLibrary Books (%d):\n", library->num_books);
    for (int i = 0; i < library->num_books; i++)
    {
        print_book(&library->books[i]);
    }
    syslog(LOG_INFO, "Listed all books in library\n");
}
