/**
 * @file main.c
 * @brief Main application entry point
 * @ingroup MangenCore
 */
#include <stdio.h>

#include "mangen.h"

/**
 * @brief Program entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return EXIT_SUCCESS on normal termination
 * @return EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
  if (argc > MAX_ARGC) {
    display_error("Wrong number of arguments.\n");
    display_help();
    return EXIT_FAILURE;
  }

  /* Scan based on DEFAULT_PATH */
  if (argc == 1) {
    scan_dir(DEFAULT_PATH, NULL, NULL);
    return EXIT_SUCCESS;
  }

  /* Scan specified PATH or display INFO flags*/
  if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      display_help();
    } else if (strcmp(argv[1], "-v") == 0) {
      display_version();
    } else {
      scan_dir(argv[1], NULL, NULL);
    }
    return EXIT_SUCCESS;
  }

  /* Scan with exclude pattern */
  int pattern_pos = 0; /* index of pattern inside argv */
  bool default_path = true;
  if (argc == 3 && strcmp(argv[1], "-e") == 0) {
    pattern_pos = 2;
  } else if (argc == 4 && strcmp(argv[2], "-e") == 0) {
    pattern_pos = 3;
    default_path = false;
  } else {
    display_error("Wrong argument sequence.\n");
    display_help();
    exit(EXIT_FAILURE);
  }

  const char *path = (default_path) ? DEFAULT_PATH : argv[1];
  const char *pattern = argv[pattern_pos];
  return process_exclude_scan(path, pattern);
}
