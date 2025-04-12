/**
 * @file main.c
 * @brief Main application entry point
 * @ingroup MangenCore
 */
#include "mangen.h"
#include <stdio.h>

/**
 * @brief Program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return EXIT_SUCCESS on normal termination
 * @return EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
  // const char *dirpath = NULL;
  if (argc > MAX_ARGC) {
    display_error("Wrong number of arguments.\n");
    display_help();
    exit(EXIT_FAILURE);
  }

  if (argc == 1) {
    scan_dir(DEFAULT_PATH, NULL);
  }

  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      display_help();
    } else if (strcmp(argv[1], "-v") == 0) {
      display_version();
    } else {
      scan_dir(argv[1], NULL);
    }
  }

  return EXIT_SUCCESS;
}
