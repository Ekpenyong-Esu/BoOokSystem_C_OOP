// test_library_management.c
#include "unity.h"
#include "library_management.h"
#include "book_management.h"
#include "member_management.h"

#include <stdbool.h>

void setUp(void) {
    // Setup runs before each test
}

void tearDown(void) {
    // Cleanup runs after each test
}


// Test: Initialization with valid pointer
void test_init_library_with_valid_pointer(void) {
    Library library;
    init_library(&library);

    // Assert
    TEST_ASSERT_NOT_NULL(library.books);
    TEST_ASSERT_NOT_NULL(library.members);
    TEST_ASSERT_EQUAL_INT(0, library.num_books);
    TEST_ASSERT_EQUAL_INT(0, library.num_members);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, library.capacity_books);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, library.capacity_members);

    deinit_library(&library);
}

// Test: Deinitialization with empty library
void test_deinit_library_with_empty_library(void) {
    Library library = {0};
    init_library(&library);

    deinit_library(&library);

    // Assert
    TEST_ASSERT_NULL(library.books);
    TEST_ASSERT_NULL(library.members);
    TEST_ASSERT_EQUAL_INT(0, library.num_books);
    TEST_ASSERT_EQUAL_INT(0, library.num_members);
    TEST_ASSERT_EQUAL_INT(0, library.capacity_books);
    TEST_ASSERT_EQUAL_INT(0, library.capacity_members);
}


// Test: Create library
void test_create_library(void) {
    Library* library = create_library();

    // Assert
    TEST_ASSERT_NOT_NULL(library);
    TEST_ASSERT_NOT_NULL(library->books);
    TEST_ASSERT_NOT_NULL(library->members);
    TEST_ASSERT_EQUAL_INT(0, library->num_books);
    TEST_ASSERT_EQUAL_INT(0, library->num_members);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, library->capacity_books);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, library->capacity_members);

    delete_library(library);
}

// Test: Delete library
void test_delete_library(void) {
    Library library = {0};
    init_library(&library);

    delete_library(&library);

    // Assert
    TEST_ASSERT_NULL(library.books);
    TEST_ASSERT_NULL(library.members);
    TEST_ASSERT_EQUAL_INT(0, library.num_books);
    TEST_ASSERT_EQUAL_INT(0, library.num_members);
    TEST_ASSERT_EQUAL_INT(0, library.capacity_books);
    TEST_ASSERT_EQUAL_INT(0, library.capacity_members);
}

void test_save_library_to_file_with_null_library_pointer(void) {
    const char *filename = "test_library.dat";

    // Act
    int result = save_library_to_file(NULL, filename);

    // Assert
    TEST_ASSERT_EQUAL(0, result);
}

void test_save_library_to_file_with_null_filename_pointer(void) {
    Library library = {0};
    init_library(&library);

    // Act
    int result = save_library_to_file(&library, NULL);

    // Assert
    TEST_ASSERT_EQUAL(0, result);

    deinit_library(&library);
}


void test_save_library_to_file_with_null_library_and_filename_pointers(void) {
    // Act
    int result = save_library_to_file(NULL, NULL);

    // Assert
    TEST_ASSERT_EQUAL(0, result);
}

void test_save_library_to_file_with_empty_library(void) {
    const char *filename = "test_empty_library.dat";
    Library library = {0};
    init_library(&library);

    // Act
    int result = save_library_to_file(&library, filename);

    // Assert
    TEST_ASSERT_EQUAL(1, result);

    FILE *file = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(file);

    int num_books = -1;
    int num_members = -1;
    fread(&num_books, sizeof(int), 1, file);
    fread(&num_members, sizeof(int), 1, file);

    TEST_ASSERT_EQUAL(0, num_books);
    TEST_ASSERT_EQUAL(0, num_members);

    fclose(file);
    remove(filename);
    deinit_library(&library);
}

void test_save_library_to_file_with_books_and_members(void) {
    const char *filename = "test_library_with_data.dat";
    Library library = {0};
    init_library(&library);

    // Add a book
    Book book;
    init_book(&book, "Test Book", "Test Author", "1234567890");
    library.books[0] = book;
    library.num_books = 1;

    // Add a member
    Member member;
    init_member(&member, "Test Member", "test@example.com");
    library.members[0] = member;
    library.num_members = 1;

    // Act
    int result = save_library_to_file(&library, filename);

    // Assert
    TEST_ASSERT_EQUAL(1, result);

    FILE *file = fopen(filename, "rb");
    TEST_ASSERT_NOT_NULL(file);

    int num_books = -1;
    int num_members = -1;
    fread(&num_books, sizeof(int), 1, file);
    fread(&num_members, sizeof(int), 1, file);

    TEST_ASSERT_EQUAL(1, num_books);
    TEST_ASSERT_EQUAL(1, num_members);

    Book read_book;
    fread(&read_book, sizeof(Book), 1, file);
    TEST_ASSERT_EQUAL_STRING("Test Book", read_book.title);
    TEST_ASSERT_EQUAL_STRING("Test Author", read_book.author);
    TEST_ASSERT_EQUAL_STRING("1234567890", read_book.isbn);

    Member read_member;
    fread(&read_member, sizeof(Member), 1, file);
    TEST_ASSERT_EQUAL_STRING("Test Member", read_member.name);
    TEST_ASSERT_EQUAL_STRING("test@example.com", read_member.email);

    fclose(file);
    remove(filename);
    deinit_library(&library);
}

void test_load_library_from_file_with_null_filename_pointer(void) {
    // Act
    Library *library = load_library_from_file(NULL);

    // Assert
    TEST_ASSERT_NULL(library);
}

void test_load_library_from_file_with_unreadable_file(void) {
    // Arrange
    const char *filename = "non_existent_file.dat";

    // Act
    Library *library = load_library_from_file(filename);

    // Assert
    TEST_ASSERT_NULL(library);
}

void test_load_library_from_file_with_no_books_and_no_members(void) {
    const char *filename = "test_empty_library.dat";
    Library library = {0};
    init_library(&library);

    // Save an empty library to the file
    save_library_to_file(&library, filename);

    // Act
    Library *loaded_library = load_library_from_file(filename);

    // Assert
    TEST_ASSERT_NOT_NULL(loaded_library);
    TEST_ASSERT_EQUAL_INT(0, loaded_library->num_books);
    TEST_ASSERT_EQUAL_INT(0, loaded_library->num_members);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, loaded_library->capacity_books);
    TEST_ASSERT_EQUAL_INT(INITIAL_CAPACITY, loaded_library->capacity_members);

    // Cleanup
    delete_library(loaded_library);
    remove(filename);
    deinit_library(&library);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_library_with_valid_pointer);
    RUN_TEST(test_deinit_library_with_empty_library);
    RUN_TEST(test_create_library);
    RUN_TEST(test_delete_library);
    RUN_TEST(test_save_library_to_file_with_null_library_pointer);
    RUN_TEST(test_save_library_to_file_with_null_filename_pointer);
    RUN_TEST(test_save_library_to_file_with_null_library_and_filename_pointers);
    RUN_TEST(test_save_library_to_file_with_empty_library);
    RUN_TEST(test_save_library_to_file_with_books_and_members);
    RUN_TEST(test_load_library_from_file_with_null_filename_pointer);
    RUN_TEST(test_load_library_from_file_with_unreadable_file);
    RUN_TEST(test_load_library_from_file_with_no_books_and_no_members);
    return UNITY_END();
}
