#ifndef MEMBER_MANAGEMENT_H
#define MEMBER_MANAGEMENT_H

#include "../include/structures.h"

void reset_next_member_id(void);
void init_member(Member *member, const char *name, const char *email);
void deinit_member(Member *member);
Member *create_member(const char *name, const char *email);
void delete_member(Member *member);
void print_member(const Member *member);
int add_member_to_library(Library *library,
                          const char *name,
                          const char *email);
Member *find_member_by_id(Library *library, int identity);
void remove_member_from_library(Library *library, int identity);
void list_all_members(const Library *library);
int borrow_book(Library *library, int member_id, int book_id);
int return_book(Library *library, int member_id, int book_id);

#endif
