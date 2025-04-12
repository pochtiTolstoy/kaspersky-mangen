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
  if (argc > MAX_ARGC) {
    display_error("Wrong number of arguments.\n");
    display_help();
    return EXIT_FAILURE;
  }

  if (argc == 1) {
    scan_dir(DEFAULT_PATH, NULL, NULL);
    return EXIT_SUCCESS;
  }

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

  if (argc == 3) {
    if (strcmp(argv[1], "-e") == 0) {
      /* Convert wildcard pattern into a valid regular expression string */
      regex_t exclude_regex;
      char *exclude_path = convert_pattern(argv[2]);
      int ret = regcomp(&exclude_regex, exclude_path, REG_EXTENDED);
      if (ret != 0) {
        display_error("Regex compilation failure\n");
      } else {
        scan_dir(DEFAULT_PATH, NULL, &exclude_regex);
        regfree(&exclude_regex);
      }
      free(exclude_path);
    } else {
      display_error("Wrong argument sequence.\n");
      display_help();
      return EXIT_FAILURE;
    }
  }

  if (argc == 4) {
    if (strcmp(argv[2], "-e") == 0) {
      /* Convert wildcard pattern into a valid regular expression string */
      regex_t exclude_regex;
      char *exclude_path = convert_pattern(argv[3]);
      int ret = regcomp(&exclude_regex, exclude_path, REG_EXTENDED);
      if (ret != 0) {
        display_error("Regex compilation failure\n");
      } else {
        scan_dir(argv[1], NULL, &exclude_regex);
        regfree(&exclude_regex);
      }
      free(exclude_path);
    } else {
      display_error("Wrong argument sequence.\n");
      display_help();
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
