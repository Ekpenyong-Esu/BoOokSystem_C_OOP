#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "unity.h"

#include "book_management.h"
#include "unity_internals.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void test_init_book_with_valid_inputs(void)
{
    // Arrange
    reset_book_id();
    Book book;
    const char *title = "Test Title";
    const char *author = "Test Author";
    const char *isbn = "1234567890";

    // Act
    init_book(&book, title, author, isbn);

    // Assert
    TEST_ASSERT_EQUAL(1, book.ident);
    TEST_ASSERT_EQUAL_STRING(title, book.title);
    TEST_ASSERT_EQUAL_STRING(author, book.author);
    TEST_ASSERT_EQUAL_STRING(isbn, book.isbn);
    TEST_ASSERT_EQUAL(1, book.is_available);
    TEST_ASSERT_NOT_EQUAL(0, book.added_date);
}


void test_init_book_with_null_inputs(void)
{
    // Arrange
    reset_book_id();
    Book book;

    // Act
    init_book(&book, NULL, NULL, NULL);

    // Assert
    TEST_ASSERT_EQUAL(1, book.ident);
    TEST_ASSERT_EQUAL_STRING("", book.title);
    TEST_ASSERT_EQUAL_STRING("", book.author);
    TEST_ASSERT_EQUAL_STRING("", book.isbn);
    TEST_ASSERT_EQUAL(1, book.is_available);
    TEST_ASSERT_NOT_EQUAL(0, book.added_date);
}


void test_init_book_ensures_null_termination(void)
{
    // Arrange
    reset_book_id();
    Book book;
    char long_title[MAX_TITLE_LENGTH + 10];
    char long_author[MAX_AUTHOR_LENGTH + 10];
    char long_isbn[MAX_ISBN_LENGTH + 10];

    memset(long_title, 'A', sizeof(long_title));
    memset(long_author, 'B', sizeof(long_author));
    memset(long_isbn, 'C', sizeof(long_isbn));

    // Act
    init_book(&book, long_title, long_author, long_isbn);

    // Assert
    TEST_ASSERT_EQUAL_CHAR('\0', book.title[MAX_TITLE_LENGTH - 1]);
    TEST_ASSERT_EQUAL_CHAR('\0', book.author[MAX_AUTHOR_LENGTH - 1]);
    TEST_ASSERT_EQUAL_CHAR('\0', book.isbn[MAX_ISBN_LENGTH - 1]);

    TEST_ASSERT_EQUAL_CHAR('A', book.title[MAX_TITLE_LENGTH - 2]);
    TEST_ASSERT_EQUAL_CHAR('B', book.author[MAX_AUTHOR_LENGTH - 2]);
    TEST_ASSERT_EQUAL_CHAR('C', book.isbn[MAX_ISBN_LENGTH - 2]);
}


void test_init_book_sets_is_available_flag(void)
{
    // Arrange
    reset_book_id();
    Book book;
    const char *title = "Test Title";
    const char *author = "Test Author";
    const char *isbn = "1234567890";

    // Act
    init_book(&book, title, author, isbn);

    // Assert
    TEST_ASSERT_EQUAL(1, book.is_available);
}

void test_init_book_assigns_unique_identifier(void)
{
    // Arrange
    reset_book_id();
    Book book1;
    Book book2;
    const char *title = "Test Title";
    const char *author = "Test Author";
    const char *isbn = "1234567890";

    // Act
    init_book(&book1, title, author, isbn);
    init_book(&book2, title, author, isbn);

    // Assert
    TEST_ASSERT_NOT_EQUAL(book1.ident, book2.ident);
    TEST_ASSERT_EQUAL(book1.ident + 1, book2.ident);
}

void test_init_book_sets_added_date_to_current_time(void)
{
    // Arrange
    reset_book_id();
    Book book;

    // Act
    time_t before = time(NULL);
    init_book(&book, "Test Title", "Test Author", "1234567890");
    time_t after = time(NULL);

    // Assert
    TEST_ASSERT_TRUE(book.added_date >= before);
    TEST_ASSERT_TRUE(book.added_date <= after);
}

void test_create_book_with_valid_inputs(void)
{
    const char *title = "Test Title";
    const char *author = "Test Author";
    const char *isbn = "1234567890";

    Book *book = create_book(title, author, isbn);

    TEST_ASSERT_NOT_NULL(book);
    TEST_ASSERT_EQUAL_STRING(title, book->title);
    TEST_ASSERT_EQUAL_STRING(author, book->author);
    TEST_ASSERT_EQUAL_STRING(isbn, book->isbn);
    TEST_ASSERT_EQUAL(1, book->is_available);
    TEST_ASSERT_NOT_EQUAL(0, book->added_date);

    delete_book(book);
}

void test_delete_book_with_valid_pointer(void)
{
    Book *book = create_book("Test Title", "Test Author", "1234567890");
    TEST_ASSERT_NOT_NULL(book);

    delete_book(book);

    // We can't directly check if the memory was freed,
    // but we can verify that the function doesn't crash
    TEST_PASS();
}

void test_print_book_output_format(void)
{
    // Set timezone to UTC
    setenv("TZ", "UTC", 1);
    tzset();
    Book book;
    char buffer[1024] = {0};
    FILE *temp_stdout = tmpfile();
    FILE *original_stdout = stdout;

    init_book(&book, "Test Title", "Test Author", "1234567890");
    book.ident = 42;
    book.is_available = 1;
    book.added_date = 1609459200; // 2021-01-01 00:00:00 UTC

    stdout = temp_stdout;
    print_book(&book);
    stdout = original_stdout;

    rewind(temp_stdout);
    fread(buffer, sizeof(char), sizeof(buffer), temp_stdout);
    fclose(temp_stdout);

    char expected_output[1024];
    snprintf(expected_output, sizeof(expected_output),
             "-----------------\n"
             "Book ID: 42\n"
             "Title: Test Title\n"
             "Author: Test Author\n"
             "ISBN: 1234567890\n"
             "Status: Available\n"
             "Added on: Fri Jan  1 00:00:00 2021\n"
             "-----------------\n");

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_add_book_to_empty_library(void)
{
    Library library = {0};
    library.capacity_books = 1;
    library.books = malloc(sizeof(Book) * (size_t)library.capacity_books);

    const char *title = "Test Title";
    const char *author = "Test Author";
    const char *isbn = "1234567890";

    int result = add_book_to_library(&library, title, author, isbn);

    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(1, library.num_books);
    TEST_ASSERT_EQUAL_STRING(title, library.books[0].title);
    TEST_ASSERT_EQUAL_STRING(author, library.books[0].author);
    TEST_ASSERT_EQUAL_STRING(isbn, library.books[0].isbn);
    TEST_ASSERT_EQUAL(1, library.books[0].is_available);
    TEST_ASSERT_NOT_EQUAL(0, library.books[0].added_date);

    free(library.books);
}

void test_find_book_by_id_with_valid_id(void)
{
    Library library = {0};
    library.capacity_books = 3;
    library.books = malloc(sizeof(Book) * (size_t)library.capacity_books);
    library.num_books = 3;

    init_book(&library.books[0], "Book 1", "Author 1", "ISBN1");
    init_book(&library.books[1], "Book 2", "Author 2", "ISBN2");
    init_book(&library.books[2], "Book 3", "Author 3", "ISBN3");

    int target_id = library.books[1].ident;
    Book *found_book = find_book_by_id(&library, target_id);

    TEST_ASSERT_NOT_NULL(found_book);
    TEST_ASSERT_EQUAL(target_id, found_book->ident);
    TEST_ASSERT_EQUAL_STRING("Book 2", found_book->title);
    TEST_ASSERT_EQUAL_STRING("Author 2", found_book->author);
    TEST_ASSERT_EQUAL_STRING("ISBN2", found_book->isbn);

    free(library.books);
}

void test_remove_book_from_library_with_multiple_books(void)
{
    // Arrange
    reset_book_id();
    Library library = {0};
    library.capacity_books = 3;
    library.books = malloc(sizeof(Book) * (size_t)library.capacity_books);
    if (!library.books) {
        TEST_FAIL_MESSAGE("Memory allocation failed");
        return;
    }

     // Zero initialize allocated memory
    memset(library.books, 0, sizeof(Book) * (size_t)library.capacity_books);
    library.num_books = 3;

    init_book(&library.books[0], "Book 1", "Author 1", "ISBN1");
    init_book(&library.books[1], "Book 2", "Author 2", "ISBN2");
    init_book(&library.books[2], "Book 3", "Author 3", "ISBN3");

    int target_id = library.books[1].ident;
    remove_book_from_library(&library, target_id);

    TEST_ASSERT_EQUAL(2, library.num_books);
    TEST_ASSERT_NOT_NULL(library.books);
    TEST_ASSERT_EQUAL_STRING("Book 1", library.books[0].title);
    TEST_ASSERT_EQUAL_STRING("Book 3", library.books[1].title);

    Book *removed_book = find_book_by_id(&library, target_id);
    TEST_ASSERT_NULL(removed_book);

    free(library.books);
    library.books = NULL;
    library.num_books = 0;
    library.capacity_books = 0;
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_init_book_with_valid_inputs);
    RUN_TEST(test_init_book_with_null_inputs);
    RUN_TEST(test_init_book_ensures_null_termination);
    RUN_TEST(test_init_book_sets_is_available_flag);
    RUN_TEST(test_init_book_assigns_unique_identifier);
    RUN_TEST(test_init_book_sets_added_date_to_current_time);
    RUN_TEST(test_create_book_with_valid_inputs);
    RUN_TEST(test_delete_book_with_valid_pointer);
    RUN_TEST(test_print_book_output_format);
    RUN_TEST(test_add_book_to_empty_library);
    RUN_TEST(test_find_book_by_id_with_valid_id);
    RUN_TEST(test_remove_book_from_library_with_multiple_books);
    return UNITY_END();
}
