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
  printf("[HELP]: ./mangen [DIRECTORY] [OPTIONS]\n"
         "Calculate SHA256 hashes for files in DIRECTORY (default: current "
         "directory)\n\n"
         "Options:\n"
         "  -h\tDisplay help message\n"
         "  -v\tDisplay version info\n");
}

void display_version() {
#ifdef GIT_COMMIT_HASH
  printf("[VERSION]: git commit hash : %s\n", GIT_COMMIT_HASH);
#else
  printf("[VERSION]: version : %s\n", "v1.0");
#endif
}

void display_error(const char *err) {
  //
  printf("[ERROR]: %s", err);
}

void scan_dir(const char *rootpath, const char *subpath) {
  struct dirent **namelist;
  char scanpath[PATH_MAX];
  if (construct_path(scanpath, PATH_MAX, rootpath, subpath) == RESULT_ERR) {
    display_error("Path length reached its maximum\n");
    return;
  }

  int file_counter = scandir(scanpath, &namelist, filter, alphasort);
  if (file_counter == -1) {
    display_error("Can't open directory.\n");
    return;
  }

  list_dir(rootpath, subpath, namelist, file_counter);
  clear_namelist(namelist, file_counter);
}

void list_dir(const char *rootpath, const char *subpath,
              struct dirent **namelist, int file_counter) {
  for (int i = 0; i < file_counter; ++i) {
    char new_subpath[PATH_MAX];
    char fullpath[PATH_MAX];

    if (construct_path(new_subpath, PATH_MAX, subpath, namelist[i]->d_name) ==
        RESULT_ERR) {
      display_error("Path length reached its maximum\n");
      return;
    }

    if (construct_path(fullpath, PATH_MAX, rootpath, new_subpath) ==
        RESULT_ERR) {
      display_error("Path length reached its maximum\n");
      return;
    }

    if (namelist[i]->d_type == DT_DIR) {
      scan_dir(rootpath, new_subpath);
    } else if (namelist[i]->d_type == DT_REG) {
      printf("%s : ", new_subpath);
      print_hash(fullpath);
    } else {
      char err_msg[PATH_MAX];
      snprintf(err_msg, PATH_MAX, "Unknown type of file : %s\n",
               namelist[i]->d_name);
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

void print_hash(const char *fullpath) {
  char hash_str[EVP_MAX_MD_SIZE * 2 + 1];
  if (hashgen(hash_str, fullpath) == 0) {
    printf("%s", hash_str);
  }
  printf("\n");
}

int hashgen(char *hash_str, const char *filepath) {
  FILE *file = fopen(filepath, "rb");
  if (!file) {
    char err_msg[PATH_MAX];
    snprintf(err_msg, PATH_MAX, "Can't open file : %s\n", filepath);
    display_error(err_msg);
    return RESULT_ERR;
  }

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_sha256();
  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len;

  if (!EVP_DigestInit_ex(ctx, md, NULL)) {
    ERR_print_errors_fp(stderr);
    fclose(file);
    EVP_MD_CTX_free(ctx);
    return RESULT_ERR;
  }

  unsigned char buffer[4096];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    if (!EVP_DigestUpdate(ctx, buffer, bytes_read)) {
      ERR_print_errors_fp(stderr);
      break;
    }
  }

  if (EVP_DigestFinal_ex(ctx, hash, &hash_len)) {
    for (size_t i = 0; i < hash_len; ++i) {
      sprintf(hash_str + (i * 2), "%02x", hash[i]);
    }
    hash_str[hash_len * 2] = '\0';
  }

  fclose(file);
  EVP_MD_CTX_free(ctx);

  return RESULT_OK;
}

Result construct_path(char *result, int max_size, const char *base,
                      const char *sub) {
  Result err = RESULT_OK;
  const int is_base = (base != NULL && base[0] != '\0');
  const int is_sub = (sub != NULL && sub[0] != '\0');
  int written = 0;
  if (is_base && is_sub) {
    written = snprintf(result, max_size, "%s/%s", base, sub);
  } else if (is_base) {
    written = snprintf(result, max_size, "%s", base);
  } else if (is_sub) {
    written = snprintf(result, max_size, "%s", sub);
  }

  if ((!is_base && !is_sub) || written < 0 || written >= max_size) {
    result[0] = '\0';
    err = RESULT_ERR;
  }

  return err;
}
