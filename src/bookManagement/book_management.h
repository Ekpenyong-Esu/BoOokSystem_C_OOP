// include/book_management.h
#ifndef BOOK_MANAGEMENT_H
#define BOOK_MANAGEMENT_H

#include "../include/structures.h"

void init_book(Book *book,
               const char *title,
               const char *author,
               const char *isbn);
void deinit_book(Book *book);
Book *create_book(const char *title, const char *author, const char *isbn);
void delete_book(Book *book);
void print_book(const Book *book);
int add_book_to_library(Library *library,
                        const char *title,
                        const char *author,
                        const char *isbn);
Book *find_book_by_id(Library *library, int identity);
void remove_book_from_library(Library *library, int identity);
void list_all_books(const Library *library);
Book *search_books(const Library *library, const char *query, int *num_results);

#endif
