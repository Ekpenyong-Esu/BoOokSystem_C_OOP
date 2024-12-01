#include "library_management.h"
#include "../bookManagement/book_management.h"
#include "../memberManagement/member_management.h"
#include <stdio.h>
#include <stdlib.h>


void init_library(Library *library)
{
    if (!library)
    {
        fprintf(stderr, "Init Library pointer is NULL\n");
        syslog(LOG_ERR, "Init Library pointer is NULL\n");
        return;
    }
    library->books = (Book *)malloc(INITIAL_CAPACITY * sizeof(Book));
    library->members = (Member *)malloc(INITIAL_CAPACITY * sizeof(Member));

    if (!library->books || !library->members)
    {
        syslog(LOG_ERR, "Memory allocation failed for library contents\n");
        free(library->books);
        free(library->members);
        library->books = NULL;
        library->members = NULL;
        return;
    }

    library->num_books = 0;
    library->capacity_books = INITIAL_CAPACITY;
    library->num_members = 0;
    library->capacity_members = INITIAL_CAPACITY;
}

void deinit_library(Library *library)
{
    if (!library)
    {
        fprintf(stderr, "Deinit Library pointer is NULL\n");
        syslog(LOG_ERR, "Deinit Library pointer is NULL\n");
        return;
    }

    for (int i = 0; i < library->num_books; i++)
    {
        deinit_book(&library->books[i]);
    }

    for (int i = 0; i < library->num_members; i++)
    {
        deinit_member(&library->members[i]);
    }

    free(library->books);
    free(library->members);

    library->books = NULL;
    library->members = NULL;
    library->num_books = 0;
    library->num_members = 0;
    library->capacity_books = 0;
    library->capacity_members = 0;
}

Library *create_library(void)
{
    Library *library = (Library *)malloc(sizeof(Library));
    if (!library)
    {
        fprintf(stderr, "Failed to allocate memory for library\n");
        syslog(LOG_ERR, "Failed to allocate memory for library\n");
        return NULL;
    }

    init_library(library);
    if (!library->books || !library->members)
    {
        fprintf(stderr, "Failed to initialize library contents\n");
        syslog(LOG_ERR, "Failed to initialize library contents\n");
        free(library);
        return NULL;
    }

    return library;
}

void delete_library(Library *library)
{
    if (!library)
    {
        fprintf(stderr, "Delete Library pointer is NULL\n");
        syslog(LOG_ERR, "Delete Library pointer is NULL\n");
        return;
    }
    deinit_library(library);
   // free(library);
}

int save_library_to_file(const Library *library, const char *filename)
{
    if (!library || !filename)
    {
        fprintf(stderr,
                "Save Library to File Library or Filename pointer is NULL\n");
        syslog(LOG_ERR,
               "Save Library to File Library or Filename pointer is NULL\n");
        return 0;
    }

    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for writing\n");
        syslog(LOG_ERR, "Failed to open file for writing\n");
        return 0;
    }

    fwrite(&library->num_books, sizeof(int), 1, file);
    fwrite(&library->num_members, sizeof(int), 1, file);
    fwrite(library->books, sizeof(Book), (size_t)library->num_books, file);
    fwrite(library->members,
           sizeof(Member),
           (size_t)library->num_members,
           file);

    fclose(file);
    return 1;
}

Library *load_library_from_file(const char *filename)
{
    if (!filename)
    {
        fprintf(stderr, "Load Library from File Filename pointer is NULL\n");
        syslog(LOG_ERR, "Load Library from File Filename pointer is NULL\n");
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for reading\n");
        syslog(LOG_ERR, "Failed to open file for reading\n");
        return NULL;
    }

    Library *library = create_library();
    if (!library)
    {
        fprintf(stderr, "Failed to create library\n");
        syslog(LOG_ERR, "Failed to create library\n");
        fclose(file);
        return NULL;
    }

    int num_books = -1;
    int num_members = -1;

    if (fread(&num_books, sizeof(int), 1, file) != 1 ||
        fread(&num_members, sizeof(int), 1, file) != 1)
    {
        fprintf(stderr, "Failed to read number of books and members\n");
        syslog(LOG_ERR, "Failed to read number of books and members\n");
        delete_library(library);
        fclose(file);
        return NULL;
    }

    // Ensure capacity for loaded data
    while (library->capacity_books < num_books)
    {
        library->capacity_books *= 2;
        Book *new_books =
            realloc(library->books,
                    (size_t)library->capacity_books * sizeof(Book));
        if (!new_books)
        {
            fprintf(stderr, "Failed to allocate memory for books\n");
            syslog(LOG_ERR, "Failed to allocate memory for books\n");
            delete_library(library);
            fclose(file);
            return NULL;
        }
        library->books = new_books;
    }

    while (library->capacity_members < num_members)
    {
        library->capacity_members *= 2;
        Member *new_members =
            realloc(library->members,
                    (size_t)library->capacity_members * sizeof(Member));
        if (!new_members)
        {
            fprintf(stderr, "Failed to allocate memory for members\n");
            syslog(LOG_ERR, "Failed to allocate memory for members\n");
            delete_library(library);
            fclose(file);
            return NULL;
        }
        library->members = new_members;
    }

    if (fread(library->books, sizeof(Book), (size_t)num_books, file) !=
            (size_t)num_books ||
        fread(library->members, sizeof(Member), (size_t)num_members, file) !=
            (size_t)num_members)
    {
        delete_library(library);
        fclose(file);
        return NULL;
    }

    library->num_books = num_books;
    library->num_members = num_members;

    fclose(file);
    return library;
}

void print_library_statistics(const Library *library)
{
    if (!library)
    {
        fprintf(stderr, "Print Library Statistics Library pointer is NULL\n");
        syslog(LOG_ERR, "Print Library Statistics Library pointer is NULL\n");
        return;
    }
    printf("\nLibrary Statistics:\n");
    printf("Total Books: %d\n", library->num_books);
    printf("Total Members: %d\n", library->num_members);

    int available_books = 0;
    for (int i = 0; i < library->num_books; i++)
    {
        if (library->books[i].is_available)
        {
            available_books++;
        }
    }

    printf("Available Books: %d\n", available_books);
    printf("Borrowed Books: %d\n", library->num_books - available_books);
    printf("-----------------\n");
}
