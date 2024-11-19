#include "book_management.h"
#include "library_management.h"
#include "member_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void open_syslog_connection(void)
{
    openlog("LibraryManagementSystem", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Library Management System started");
}

void clear_input_buffer(void)
{
    int character = '\0';
    while ((character = getchar()) != '\n' && character != EOF)
    {
        // discard characters
    }
}

void read_string(char *buffer, int max_length, const char *prompt)
{
    printf("%s", prompt);
    if (fgets(buffer, max_length, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
}

void print_menu(void)
{
    printf("\nLibrary Management System\n");
    printf("========================\n");
    printf("1. Add Book\n");
    printf("2. Add Member\n");
    printf("3. Borrow Book\n");
    printf("4. Return Book\n");
    printf("5. List All Books\n");
    printf("6. List All Members\n");
    printf("7. Remove Book\n");
    printf("8. Remove Member\n");
    printf("9. Library Statistics\n");
    printf("10. Save and Exit\n");
    printf("Choose an option: ");
}

int main(void)
{
    Library *library = NULL;

    open_syslog_connection();

    // Try to load existing library data
    library = load_library_from_file("library.dat");
    if (library == NULL)
    {
        printf("Creating new library...\n");
        library = create_library();
        if (!library)
        {
            fprintf(stderr, "Failed to create library\n");
            fprintf(stderr, "Failed to initialize library\n");
            return 1;
        }
    }

    int choice = 0;
    char title[MAX_TITLE_LENGTH];
    char author[MAX_AUTHOR_LENGTH];
    char isbn[MAX_ISBN_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int member_id = 0;
    int book_id = 0;

    while (1)
    {
        print_menu();
        if (scanf("%d", &choice) != 1)
        {
            clear_input_buffer();
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        clear_input_buffer();

        switch (choice)
        {
        case 1:
            read_string(title, MAX_TITLE_LENGTH, "Enter book title: ");
            read_string(author, MAX_AUTHOR_LENGTH, "Enter author: ");
            read_string(isbn, MAX_ISBN_LENGTH, "Enter ISBN: ");

            if (add_book_to_library(library, title, author, isbn))
            {
                printf("Book added successfully!\n");
                syslog(LOG_INFO,
                       "Book added with Title: %s, Author: %s, ISBN: %s\n",
                       title,
                       author,
                       isbn);
            }
            else
            {
                printf("Failed to add book.\n");
                syslog(
                    LOG_ERR,
                    "Failed to add book with Title: %s, Author: %s, ISBN: %s\n",
                    title,
                    author,
                    isbn);
            }
            break;

        case 2:
            read_string(name, MAX_NAME_LENGTH, "Enter member name: ");
            read_string(email, MAX_EMAIL_LENGTH, "Enter email: ");

            if (add_member_to_library(library, name, email))
            {
                printf("Member added successfully!\n");
                syslog(LOG_INFO,
                       "Member added with Name: %s, Email: %s\n",
                       name,
                       email);
            }
            else
            {
                printf("Failed to add member.\n");
                syslog(LOG_ERR,
                       "Failed to add member with Name: %s, Email: %s\n",
                       name,
                       email);
            }
            break;

        case 3:
            printf("Enter member ID: ");
            scanf("%d", &member_id);
            printf("Enter book ID: ");
            scanf("%d", &book_id);
            clear_input_buffer();

            if (borrow_book(library, member_id, book_id))
            {
                printf("Book borrowed successfully!\n");
                syslog(LOG_INFO,
                       "Book borrowed by Member ID: %d, Book ID: %d\n",
                       member_id,
                       book_id);
            }
            else
            {
                printf("Failed to borrow book.\n");
                syslog(LOG_ERR,
                       "Failed to borrow book by Member ID: %d, Book ID: %d\n",
                       member_id,
                       book_id);
            }
            break;

        case 4:
            printf("Enter member ID: ");
            scanf("%d", &member_id);
            printf("Enter book ID: ");
            scanf("%d", &book_id);
            clear_input_buffer();

            if (return_book(library, member_id, book_id))
            {
                printf("Book returned successfully!\n");
                syslog(LOG_INFO,
                       "Book returned by Member ID: %d, Book ID: %d\n",
                       member_id,
                       book_id);
            }
            else
            {
                printf("Failed to return book.\n");
                syslog(LOG_ERR,
                       "Failed to return book by Member ID: %d, Book ID: %d\n",
                       member_id,
                       book_id);
            }
            break;

        case 5:
            list_all_books(library);
            break;

        case 6:
            list_all_members(library);
            break;

        case 7:
            printf("Enter book ID to remove: ");
            scanf("%d", &book_id);
            clear_input_buffer();
            remove_book_from_library(library, book_id);
            printf("Book removed if it existed.\n");
            break;

        case 8:
            printf("Enter member ID to remove: ");
            scanf("%d", &member_id);
            clear_input_buffer();
            remove_member_from_library(library, member_id);
            printf("Member removed if they existed.\n");
            break;

        case 9:
            print_library_statistics(library);
            break;

        case 10:
            if (save_library_to_file(library, "library.dat"))
            {
                printf("Library data saved successfully!\n");
                syslog(LOG_INFO, "Library data saved to file\n");
            }
            else
            {
                printf("Failed to save library data.\n");
                syslog(LOG_ERR, "Failed to save library data to file\n");
            }
            delete_library(library);
            return 0;

        default:
            printf("Invalid option. Please try again.\n");
            syslog(LOG_ERR, "Invalid option selected\n");
        }
    }

    return 0;
}
