/**
 * @file mangen.c
 * @brief Implementation of file hash generator core functionality
 * @ingroup MangenCore
 */
#include "mangen.h"
#include <limits.h> // For PATH_MAX

/* Helper macro for error message formatting */
#define ERROR_BUF_SIZE (PATH_MAX + 64)

void display_help() {
  printf("[HELP]: ./mangen [DIRECTORY] [OPTIONS]\n"
         "Calculate SHA256 hashes for files in DIRECTORY (default: current "
         "directory)\n\n"
         "Options:\n"
         "  -e\tExclude all files/directories with the name FILE_NAME\n"
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

void display_error(const char *err) { printf("[ERROR]: %s", err); }

void scan_dir(const char *rootpath, const char *subpath,
              regex_t *exclude_regex) {
  struct dirent **namelist;
  char scanpath[PATH_MAX];

  /* Construct full path */
  if (construct_path(scanpath, PATH_MAX, rootpath, subpath) == RESULT_ERR) {
    display_error("Path length reached its maximum\n");
    return;
  }

  /* Scan directory with filtering and sorted output */
  int file_counter = scandir(scanpath, &namelist, filter, alphasort);
  if (file_counter == -1) {
    char err_msg[PATH_MAX + 64];
    snprintf(err_msg, PATH_MAX + 64, "Can't open directory : %s\n", scanpath);
    display_error(err_msg);
    return;
  }

  /* Process directory contents and clean up */
  list_dir(rootpath, subpath, namelist, file_counter, exclude_regex);
  clear_namelist(namelist, file_counter);
}

void list_dir(const char *rootpath, const char *subpath,
              struct dirent **namelist, int file_counter,
              regex_t *exclude_regex) {
  /* Iterate through directory entries with path construction */
  for (int i = 0; i < file_counter; ++i) {
    char new_subpath[PATH_MAX];
    char fullpath[PATH_MAX];

    /* Build relative path for current entry */
    if (construct_path(new_subpath, PATH_MAX, subpath, namelist[i]->d_name) ==
        RESULT_ERR) {
      display_error("Path length reached its maximum\n");
      return;
    }

    /* Build absolute path for current entry */
    if (construct_path(fullpath, PATH_MAX, rootpath, new_subpath) ==
        RESULT_ERR) {
      display_error("Path length reached its maximum\n");
      return;
    }

    /* Filter current entry path */
    if (is_exclude_path(new_subpath, exclude_regex)) {
      continue;
    }

    /* Handle differet file types */
    if (namelist[i]->d_type == DT_DIR) {
      scan_dir(rootpath, new_subpath, exclude_regex); // Recursive descent
    } else if (namelist[i]->d_type == DT_REG) {
      printf("%s : ", new_subpath);
      fflush(stdout);
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
  /* Save memory cleanup for scandir results */
  for (int i = 0; i < n; ++i) {
    free(namelist[i]);
  }
  free(namelist);
}

void print_hash(const char *fullpath) {
  char hash_str[EVP_MAX_MD_SIZE * 2 + 1];
  if (hashgen(hash_str, fullpath) == RESULT_OK) {
    printf("%s\n", hash_str);
  }
}

Result hashgen(char *hash_str, const char *filepath) {
  /* SHA256 generation using OpenSSL EVP interface */
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

  /* Digest initialization */
  if (!EVP_DigestInit_ex(ctx, md, NULL)) {
    ERR_print_errors_fp(stderr);
    fclose(file);
    EVP_MD_CTX_free(ctx);
    return RESULT_ERR;
  }

  /* Chunked file reading for memory efficiency */
  unsigned char buffer[4096];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
    if (!EVP_DigestUpdate(ctx, buffer, bytes_read)) {
      ERR_print_errors_fp(stderr);
      break;
    }
  }

  /* Finalize digest and convert to hex string */
  if (EVP_DigestFinal_ex(ctx, hash, &hash_len)) {
    for (size_t i = 0; i < hash_len; ++i) {
      sprintf(hash_str + (i * 2), "%02x", hash[i]);
    }
    hash_str[hash_len * 2] = '\0';
  }

  /* Resource cleanup */
  fclose(file);
  EVP_MD_CTX_free(ctx);

  return RESULT_OK;
}

Result construct_path(char *result, int max_size, const char *base,
                      const char *sub) {
  /* Save path concatentation with edge case handling */
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

  /* Error condition checking */
  if ((!is_base && !is_sub) || written < 0 || written >= max_size) {
    result[0] = '\0';
    err = RESULT_ERR;
  }

  return err;
}

int filter(const struct dirent *file) {
  /* Directory entry filter */
  return (strcmp(file->d_name, ".") && strcmp(file->d_name, ".."));
}

int is_exclude_path(const char *src, regex_t *exclude_regex) {
  if (!src || !exclude_regex) {
    return 0;
  }

  /* Extract file name from full path */
  const char *base = strrchr(src, '/');
  base = (base) ? (base + 1) : src;

  /* Execute the regular expression on the base name */
  return (regexec(exclude_regex, base, 0, NULL, 0) == 0);
}

char *convert_pattern(const char *pattern) {
  size_t len = strlen(pattern);
  size_t buf_size = 2 * len + 10; // enough size for transformation
  char *regex_pattern = malloc(buf_size);
  if (!regex_pattern) {
    return NULL;
  }
  char *ptr = regex_pattern;
  *ptr++ = '^'; // Start of string anchor

  for (size_t i = 0; i < len; ++i) {
    if (pattern[i] == '*') {
      *ptr++ = '.';
      *ptr++ = '*';
    } else {
      *ptr++ = pattern[i];
    }
  }
  *ptr++ = '$'; // End of string anchor
  *ptr = '\0';

  return regex_pattern;
}

int process_exclude_scan(const char *path, const char *pattern) {
  /* Convert wildcard pattern into a valid regular expression string */
  regex_t exclude_regex;
  char *exclude_pattern = convert_pattern(pattern);
  int ret = regcomp(&exclude_regex, exclude_pattern, REG_EXTENDED);
  if (ret != 0) {
    display_error("Regex compilation failure\n");
  } else {
    scan_dir(path, NULL, &exclude_regex); /* process directory */
    regfree(&exclude_regex);
  }
  free(exclude_pattern);
  return ret;
}
