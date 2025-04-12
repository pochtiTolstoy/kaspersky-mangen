#ifndef MANGEN_H
#define MANGEN_H

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGC 2
#define DEFAULT_PATH "."

typedef enum { RESULT_OK, RESULT_ERR } Result;

int filter(const struct dirent *);
void display_help();
void display_version();
void display_error(const char *);
void scan_dir(const char *, const char *);
void list_dir(const char *, const char *, struct dirent **, int);
void clear_namelist(struct dirent **, int);
void print_hash(const char *);
int hashgen(char *, const char *);
Result construct_path(char *, int, const char *, const char *);

#endif /* MANGEN_H */
