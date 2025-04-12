#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGC 2
#define DEFAULT_PATH "."

int filter(const struct dirent *);
void display_help();
void display_version();
void display_error(const char *);
void scan_dir(const char *, const char *);
void list_dir(const char *, const char *, struct dirent **, int);
void clear_namelist(struct dirent **, int);

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

int filter(const struct dirent *file) {
  return (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."));
}

void display_help() {
  //
  printf("[HELP]:\n");
}

void display_version() {
  //
  printf("[VERSION]:\n");
}

void display_error(const char *err) {
  //
  printf("[ERROR]: %s", err);
}

void scan_dir(const char *rootpath, const char *printpath) {
  struct dirent **namelist;
  char scanpath[PATH_MAX];
  int length;
  if (printpath) {
    length = snprintf(scanpath, PATH_MAX, "%s/%s", rootpath, printpath);
  } else {
    length = snprintf(scanpath, PATH_MAX, "%s", rootpath);
  }
  if (length >= PATH_MAX) {
    display_error("Path length reached its maximum\n");
    return;
  }
  int file_counter = scandir(scanpath, &namelist, filter, alphasort);
  if (file_counter == -1) {
    display_error("Can't open directory.\n");
    return;
  }
  list_dir(rootpath, printpath, namelist, file_counter);
  clear_namelist(namelist, file_counter);
}

void list_dir(const char *rootpath, const char *printpath,
              struct dirent **namelist, int file_counter) {
  for (int i = 0; i < file_counter; ++i) {
    if (namelist[i]->d_type == DT_DIR) {
      char new_printpath[PATH_MAX];
      int length;
      if (printpath) {
        length = snprintf(new_printpath, PATH_MAX, "%s/%s", printpath,
                          namelist[i]->d_name);
      } else {
        length = snprintf(new_printpath, PATH_MAX, "%s", namelist[i]->d_name);
      }
      if (length >= PATH_MAX) {
        display_error("Path length reached its maximum\n");
        return;
      }
      scan_dir(rootpath, new_printpath);
    } else if (namelist[i]->d_type == DT_REG) {
      (printpath) ? printf("%s/", printpath) : 0;
      printf("%s : %s\n", namelist[i]->d_name, "0");
    } else {
      char err_msg[PATH_MAX];
      snprintf(err_msg, PATH_MAX, "Unkown type of : %s\n", namelist[i]->d_name);
      display_error(err_msg);
    }
  }
}

void clear_namelist(struct dirent **namelist, int n) {
  for (int i = 0; i < n; ++i) {
    free(namelist[i]);
  }
  free(namelist);
}
