/**
 * @file utils.h
 * @brief Utility functions for bottom-up parser
 */

 #ifndef UTILS_H
 #define UTILS_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include "../include/parser.h"
 
 /**
  * @brief Safe string duplication
  * 
  * @param str String to duplicate
  * @return char* Duplicated string or NULL on failure
  */
 char* safe_strdup(const char* str);
 
 /**
  * @brief Safe memory allocation with error handling
  * 
  * @param size Size to allocate
  * @return void* Allocated memory or exit on failure
  */
 void* safe_malloc(size_t size);
 
 /**
  * @brief Safe memory reallocation with error handling
  * 
  * @param ptr Pointer to reallocate
  * @param size New size
  * @return void* Reallocated memory or exit on failure
  */
 void* safe_realloc(void* ptr, size_t size);
 
 /**
  * @brief Log error message to stderr
  * 
  * @param format Format string
  * @param ... Additional arguments
  */
 void log_error(const char* format, ...);
 
 /**
  * @brief Log debug message if debug mode enabled
  * 
  * @param debug_mode Whether debug mode is enabled
  * @param format Format string
  * @param ... Additional arguments
  */
 void log_debug(bool debug_mode, const char* format, ...);
 
 /**
  * @brief Create a new formatted string
  * 
  * @param format Format string
  * @param ... Additional arguments
  * @return char* Formatted string (must be freed by caller)
  */
 char* string_format(const char* format, ...);
 
 /**
  * @brief Append to a string, reallocating as needed
  * 
  * @param original Original string (will be freed)
  * @param append String to append
  * @return char* New combined string
  */
 char* string_append(char* original, const char* append);
 
 #endif /* UTILS_H */