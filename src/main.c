#include "mangen.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  const char *dirpath = NULL;
  if (argc > MAX_ARGC) {
    display_error("Wrong number of arguments.\n");
    display_help();
    exit(EXIT_FAILURE);
  }

  if (argc == 2 && strcmp(argv[1], "-h") == 0) {
    display_help();
  } else if (argc == 2 && strcmp(argv[1], "-v") == 0) {
    display_version();
  } else {
    dirpath = (argc == 2) ? argv[1] : DEFAULT_PATH;
    scan_dir(dirpath, NULL);
  }

  return 0;
}
