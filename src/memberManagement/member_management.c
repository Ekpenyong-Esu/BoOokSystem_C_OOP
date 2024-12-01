#include "member_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "../bookManagement/book_management.h"

static int next_member_id = 1;

void reset_next_member_id(void) {
    next_member_id = 1;
}

void init_member(Member *member, const char *name, const char *email)
{
    if (!member)
    {
        fprintf(stderr, "Init Member pointer is NULL\n");
        syslog(LOG_ERR, "Init Member pointer is NULL\n");
        return;
    }

    member->ident = next_member_id++;
    strncpy(member->name, name ? name : "", MAX_NAME_LENGTH - 1);
    strncpy(member->email, email ? email : "", MAX_EMAIL_LENGTH - 1);

    member->name[MAX_NAME_LENGTH - 1] = '\0';
    member->email[MAX_EMAIL_LENGTH - 1] = '\0';

    member->num_borrowed_books = 0;
    memset(member->borrowed_books, 0, sizeof(member->borrowed_books));
    syslog(LOG_INFO,
           "Init member with ID: %d, Name: %s, Email: %s\n",
           member->ident,
           member->name,
           member->email);
}

void deinit_member(Member *member)
{
    (void)member; // Unused parameter
    // Currently no dynamic memory to free
}

Member *create_member(const char *name, const char *email)
{
    if (!name || !email)
    {
        fprintf(stderr, "Invalid name or email\n");
        syslog(LOG_ERR, "Invalid name or email\n");
        return NULL;
    }

    Member *member = (Member *)malloc(sizeof(Member));
    if (!member)
    {
        fprintf(stderr, "Creating Memory allocation failed for member\n");
        syslog(LOG_ERR, "Creating Memory allocation failed for member\n");
        return NULL;
    }

    init_member(member, name, email);
    syslog(LOG_INFO, "Created member with Name: %s, Email: %s\n", name, email);
    return member;
}

void delete_member(Member *member)
{
    if (!member)
    {
        fprintf(stderr, "Deleting Member pointer is NULL\n");
        syslog(LOG_ERR, "Delete Member pointer is NULL\n");
        return;
    }
    deinit_member(member);
    syslog(LOG_INFO, "Deleted member with ID: %d\n", member->ident);
    free(member);
}

void print_member(const Member *member)
{
    if (!member)
    {
        fprintf(stderr, "Printing Member pointer is NULL\n");
        return;
    }
    printf("Member ID: %d\n", member->ident);
    printf("Name: %s\n", member->name);
    printf("Email: %s\n", member->email);
    printf("Borrowed Books: %d\n", member->num_borrowed_books);
    printf("-----------------\n");
    syslog(LOG_INFO, "Printed member with ID: %d\n", member->ident);
}

int add_member_to_library(Library *library, const char *name, const char *email)
{
    if (!library || !name || !email)
    {
        fprintf(stderr, "Invalid parameters for adding a member\n");
        syslog(LOG_ERR, "Invalid parameters for adding a member\n");
        return 0;
    }

    if (library->num_members >= library->capacity_members)
    {
        int new_capacity = library->capacity_members * 2;
        Member *new_members =
            realloc(library->members, (size_t)new_capacity * sizeof(Member));
        if (!new_members)
        {
            fprintf(stderr,
                    "Memory reallocation failed to add library members\n");
            syslog(LOG_ERR,
                   "Memory reallocation failed to add library members\n");
            return 0;
        }

        library->members = new_members;
        library->capacity_members = new_capacity;
    }

    init_member(&library->members[library->num_members], name, email);
    library->num_members++;
    syslog(LOG_INFO,
           "Added member to the library with Name: %s, Email: %s\n",
           name,
           email);
    return 1;
}

Member *find_member_by_id(Library *library, int ident)
{
    if (!library)
    {
        fprintf(stderr, "Find Member Library pointer is NULL\n");
        syslog(LOG_ERR, "Find Member Library pointer is NULL\n");
        return NULL;
    }

    for (int i = 0; i < library->num_members; i++)
    {
        if (library->members[i].ident == ident)
        {
            syslog(LOG_INFO, "Found member with ID: %d\n", ident);
            return &library->members[i];
        }
    }
    return NULL;
}

void remove_member_from_library(Library *library, int ident)
{
    if (!library)
    {
        fprintf(stderr, "Remove Member Library pointer is NULL\n");
        syslog(LOG_ERR, "Remove Member Library pointer is NULL\n");
        return;
    }

    int found_index = -1;
    for (int i = 0; i < library->num_members; i++)
    {
        if (library->members[i].ident == ident)
        {
            syslog(LOG_INFO, "Removing member with ID: %d\n Found", ident);
            found_index = i;
            break;
        }
    }

    if (found_index != -1)
    {
        deinit_member(&library->members[found_index]);
        syslog(LOG_INFO, "Removed member with ID: %d\n", ident);
        for (int i = found_index; i < library->num_members - 1; i++)
        {
            library->members[i] = library->members[i + 1];
        }
        library->num_members--;
    }
}

void list_all_members(const Library *library)
{
    if (!library)
    {
        fprintf(stderr, "List Members Library pointer is NULL\n");
        syslog(LOG_ERR, "List Members Library pointer is NULL\n");
        return;
    }

    printf("\nLibrary Members (%d):\n", library->num_members);
    for (int i = 0; i < library->num_members; i++)
    {
        print_member(&library->members[i]);
        syslog(LOG_INFO, "Listed all members in library\n");
    }
}

int borrow_book(Library *library, int member_id, int book_id)
{
    if (!library)
    {
        fprintf(stderr, "Borrow Book Library pointer is NULL\n");
        syslog(LOG_ERR, "Borrow Book Library pointer is NULL\n");
        return 0;
    }
    Member *member = find_member_by_id(library, member_id);
    Book *book = find_book_by_id(library, book_id);

    if (!member || !book)
    {
        fprintf(stderr, "Borrow Book Member or Book ID pointer is NULL\n");
        syslog(LOG_ERR, "Borrow Book Member or Book ID pointer is NULL\n");
        return 0;
    }

    if (!book->is_available)
    {
        fprintf(stderr, "Book is not available\n");
        syslog(LOG_ERR, "Book is not available\n");
        return 0;
    }

    if (member->num_borrowed_books >= MAX_BORROWED_BOOKS)
    {
        fprintf(stderr,
                "Member has reached maximum number of borrowed books\n");
        syslog(LOG_ERR,
               "Member has reached maximum number of borrowed books\n");
        return 0;
    }
    book->is_available = 0;
    member->borrowed_books[member->num_borrowed_books++] = book_id;

    return 1;
}

int return_book(Library *library, int member_id, int book_id)
{
    if (!library)
    {
        fprintf(stderr, "Return Book Library pointer is NULL\n");
        syslog(LOG_ERR, "Return Book Library pointer is NULL\n");
        return 0;
    }
    Member *member = find_member_by_id(library, member_id);
    Book *book = find_book_by_id(library, book_id);

    if (!member || !book)
    {
        fprintf(stderr, "Return Book Member or Book ID pointer is NULL\n");
        syslog(LOG_ERR, "Return Book Member or Book ID pointer is NULL\n");
        return 0;
    }
    int found = 0;
    for (int i = 0; i < member->num_borrowed_books; i++)
    {
        if (member->borrowed_books[i] == book_id)
        {
            syslog(LOG_INFO, "Returned book with ID: %d\n", book_id);
            for (int j = i; j < member->num_borrowed_books - 1; j++)
            {
                member->borrowed_books[j] = member->borrowed_books[j + 1];
            }
            member->num_borrowed_books--;
            book->is_available = 1;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        syslog(LOG_INFO,
               "Book with ID: %d not found in member's borrowed books\n",
               book_id);
    }

    return found;
}
