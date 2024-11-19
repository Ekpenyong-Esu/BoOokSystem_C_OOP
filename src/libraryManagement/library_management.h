#ifndef LIBRARY_MANAGEMENT_H
#define LIBRARY_MANAGEMENT_H

#include "../include/structures.h"

void init_library(Library *library);
void deinit_library(Library *library);
Library *create_library(void);
void delete_library(Library *library);
int save_library_to_file(const Library *library, const char *filename);
Library *load_library_from_file(const char *filename);
void print_library_statistics(const Library *library);

#endif
