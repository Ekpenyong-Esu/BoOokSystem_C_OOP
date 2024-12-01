#include "unity.h"
#include "member_management.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_create_member_with_null_name_and_email(void)
{
    // Act
    Member *member = create_member(NULL, NULL);

    // Assert
    TEST_ASSERT_NULL(member);
}

void test_init_member_with_empty_strings(void)
{
    // Arrange
    Member member;

    // Act
    init_member(&member, "", "");

    // Assert
    TEST_ASSERT_EQUAL(1, member.ident);
    TEST_ASSERT_EQUAL_STRING("", member.name);
    TEST_ASSERT_EQUAL_STRING("", member.email);
    TEST_ASSERT_EQUAL(0, member.num_borrowed_books);
    for (int i = 0; i < MAX_BORROWED_BOOKS; i++) {
        TEST_ASSERT_EQUAL(0, member.borrowed_books[i]);
    }
}

void test_add_member_to_library_increases_member_count(void)
{
    // Arrange
    Library library;
    library.num_members = 0;
    library.capacity_members = 2;
    library.members = (Member *)malloc((size_t)library.capacity_members * sizeof(Member));
    const char *name = "John Doe";
    const char *email = "john.doe@example.com";

    // Act
    int result = add_member_to_library(&library, name, email);

    // Assert
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(1, library.num_members);
    TEST_ASSERT_EQUAL_STRING(name, library.members[0].name);
    TEST_ASSERT_EQUAL_STRING(email, library.members[0].email);

    // Clean up
    free(library.members);
}



void test_find_member_by_id_in_null_library(void)
{
    // Act
    Member *result = find_member_by_id(NULL, 1);

    // Assert
    TEST_ASSERT_NULL(result);
}

void test_remove_member_from_library_decreases_member_count(void)
{
    // Arrange
    Library library;
    library.num_members = 2;
    library.capacity_members = 2;
    library.members = (Member *)malloc((size_t)library.capacity_members * sizeof(Member));

    init_member(&library.members[0], "Member1", "member1@example.com");
    init_member(&library.members[1], "Member2", "member2@example.com");

    int initial_member_count = library.num_members;
    int member_id_to_remove = library.members[0].ident;

    // Act
    remove_member_from_library(&library, member_id_to_remove);

    // Assert
    TEST_ASSERT_EQUAL(initial_member_count - 1, library.num_members);
    TEST_ASSERT_NOT_EQUAL(member_id_to_remove, library.members[0].ident);

    // Clean up
    free(library.members);
}

void test_borrow_book_when_member_reached_max_borrowed_books(void)
{
    // Arrange
    Library library;
    library.num_members = 1;
    library.capacity_members = 1;
    library.members = (Member *)malloc((size_t)library.capacity_members * sizeof(Member));
    init_member(&library.members[0], "Max Borrower", "max.borrower@example.com");

    // Simulate member reaching max borrowed books
    library.members[0].num_borrowed_books = MAX_BORROWED_BOOKS;
    for (int i = 0; i < MAX_BORROWED_BOOKS; i++) {
        library.members[0].borrowed_books[i] = i + 1; // Assume book IDs are 1, 2, ..., MAX_BORROWED_BOOKS
    }

    // Act
    int result = borrow_book(&library, library.members[0].ident, 999); // Attempt to borrow a new book with ID 999

    // Assert
    TEST_ASSERT_EQUAL(0, result); // Should not be able to borrow
    TEST_ASSERT_EQUAL(MAX_BORROWED_BOOKS, library.members[0].num_borrowed_books); // Ensure count remains the same

    // Clean up
    free(library.members);
}

void test_list_all_members_prints_member_details(void)
{
    reset_next_member_id();
    // Arrange
    Library library;
    library.num_members = 2;
    library.capacity_members = 2;
    library.members = (Member *)malloc((size_t)library.capacity_members * sizeof(Member));

    init_member(&library.members[0], "Alice Smith", "alice.smith@example.com");
    init_member(&library.members[1], "Bob Johnson", "bob.johnson@example.com");

    // Redirect stdout to a buffer to capture print output
    char buffer[1024];
    FILE *stream = tmpfile();
    if (!stream) {
        perror("tmpfile");
        exit(EXIT_FAILURE);
    }
    FILE *original_stdout = stdout;
    stdout = stream;

    // Act
    list_all_members(&library);

    // Flush and reset stdout
    fflush(stream);
    fseek(stream, 0, SEEK_SET);
    fread(buffer, sizeof(char), sizeof(buffer) - 1, stream);
    buffer[sizeof(buffer) - 1] = '\0';
    stdout = original_stdout;
    fclose(stream);

    // Assert
    const char *expected_output =
        "\nLibrary Members (2):\n"
        "Member ID: 1\n"
        "Name: Alice Smith\n"
        "Email: alice.smith@example.com\n"
        "Borrowed Books: 0\n"
        "-----------------\n"
        "Member ID: 2\n"
        "Name: Bob Johnson\n"
        "Email: bob.johnson@example.com\n"
        "Borrowed Books: 0\n"
        "-----------------\n";
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);

    // Clean up
    free(library.members);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_member_with_null_name_and_email);
    RUN_TEST(test_init_member_with_empty_strings);
    RUN_TEST(test_add_member_to_library_increases_member_count);
    RUN_TEST(test_find_member_by_id_in_null_library);
    RUN_TEST(test_remove_member_from_library_decreases_member_count);
    RUN_TEST(test_borrow_book_when_member_reached_max_borrowed_books);
    RUN_TEST(test_list_all_members_prints_member_details);

    return UNITY_END();
}
