/**
 * @file mangen.h
 * @brief File hash generator application header
 * @defgroup MangenCore Core Functionality
 * @{
 */
#ifndef MANGEN_H
#define MANGEN_H

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @def MAX_ARGC
 * @brief Maximum allowed number of command line arguments
 */
#define MAX_ARGC 2

/**
 * @def DEFAULT_PATH
 * @brief Default directory path when none is specified
 */
#define DEFAULT_PATH "."

/**
 * @enum Result
 * @brief Operation result status codes
 */
typedef enum { RESULT_OK, RESULT_ERR } Result;

// Function prototypes
/**
 * @brief Directory entry filter for scandir
 * @param entry Directory entry to check
 * @return 1 to keep entry, 0 to exclude
 */
int filter(const struct dirent *);

/**
 * @brief Display help information
 */
void display_help();

/**
 * @brief Display version information
 */
void display_version();

/**
 * @brief Display error message
 * @param message Error message to display
 */
void display_error(const char *message);

/**
 * @brief Recursive directory scanner
 * @param rootpath Base directory path
 * @param subpath Current subdirectory path
 */
void scan_dir(const char *rootpath, const char *subpath);

/**
 * @brief Process directory contents
 * @param rootpath Base directory path
 * @param subpath Current subdirectory path
 * @param namelist Array of directory entries
 * @param file_counter Number of entries in namelist
 */
void list_dir(const char *rootpath, const char *subpath,
              struct dirent **namelist, int file_counter);

/**
 * @brief Clean up scandir namelist
 * @param namelist Array of directory entries
 * @param file_counter Number of entries in array
 */
void clear_namelist(struct dirent **namelist, int file_counter);

/**
 * @brief Print file hash to stdout
 * @param fullpath Full path to target file
 */
void print_hash(const char *fullpath);

/**
 * @brief Generate SHA-256 hash for file
 * @param hash_str Output buffer for hash string
 * @param filepath Path to target file
 * @return RESULT_OK on success, RESULT_ERR on failure
 */
Result hashgen(char *hash_str, const char *filepath);

/**
 * @brief Safely construct filesystem paths
 * @param result Output buffer for constructed path
 * @param max_size Maximum size of result buffer
 * @param base Base path component
 * @param sub Additional path component
 * @return RESULT_OK on success, RESULT_ERR on failure
 */
Result construct_path(char *result, int max_size, const char *base,
                      const char *sub);

#endif    /* MANGEN_H */
/** @} */ // End of MangenCore group

/**
 * @mainpage File Hash Generator
 * @section intro Introduction
 * Recursive directory scanner that calculates SHA-256 hashes for all files.
 *
 * @section features Features
 * - Recursive directory traversal
 * - SHA-256 hash generation using OpenSSL
 * - Error handling and validation
 *
 * @section usage Usage
 * @code
 * ./mangen [DIRECTORY] [OPTION]
 * Options:
 *   -h  Show help message
 *   -v  Display version information
 * @endcode
 *
 * @section design Design
 * - Uses scandir for directory traversal
 * - OpenSSL EVP interface for hashing
 * - Path construction with buffer safety
 *
 * @section dependencies Dependencies
 * - OpenSSL 1.1.1 or newer
 * - C99-compliant compiler
 */
