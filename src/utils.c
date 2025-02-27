/**
 * @file utils.c
 * @brief Implementation of utility functions
 * @members: Group 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 #include "../include/utils.h"
 #include"../include/parser.h"
 
 /**
  * Safe string duplication with error handling
  */
 char* safe_strdup(const char* str) {
     if (!str) {
         return NULL;
     }
     
     char* dup = strdup(str);
     if (!dup) {
         log_error("Memory allocation failed (strdup)");
         exit(EXIT_FAILURE);
     }
     
     return dup;
 }
 
 /**
  * Safe memory allocation with error handling
  */
 void* safe_malloc(size_t size) {
     void* ptr = malloc(size);
     if (!ptr) {
         log_error("Memory allocation failed (malloc)");
         exit(EXIT_FAILURE);
     }
     
     return ptr;
 }
 
 /**
  * Safe memory reallocation with error handling
  */
 void* safe_realloc(void* ptr, size_t size) {
     void* new_ptr = realloc(ptr, size);
     if (!new_ptr) {
         log_error("Memory allocation failed (realloc)");
         exit(EXIT_FAILURE);
     }
     
     return new_ptr;
 }
 
 /**
  * Log error message to stderr
  */
 void log_error(const char* format, ...) {
     va_list args;
     va_start(args, format);
     
     fprintf(stderr, "ERROR: ");
     vfprintf(stderr, format, args);
     fprintf(stderr, "\n");
     
     va_end(args);
 }
 
 /**
  * Log debug message to stderr if debug mode is enabled
  */
 void log_debug(bool debug_mode, const char* format, ...) {
     if (!debug_mode) {
         return;
     }
     
     va_list args;
     va_start(args, format);
     
     fprintf(stderr, "DEBUG: ");
     vfprintf(stderr, format, args);
     fprintf(stderr, "\n");
     
     va_end(args);
 }
 
 /**
  * Format a string using printf-style formatting
  */
 char* string_format(const char* format, ...) {
     va_list args;
     va_start(args, format);
     
     // Determine length of formatted string
     va_list args_copy;
     va_copy(args_copy, args);
     int length = vsnprintf(NULL, 0, format, args_copy);
     va_end(args_copy);
     
     if (length < 0) {
         va_end(args);
         return safe_strdup("ERROR");
     }
     
     // Allocate buffer
     char* buffer = (char*)safe_malloc(length + 1);
     
     // Format string
     vsnprintf(buffer, length + 1, format, args);
     va_end(args);
     
     return buffer;
 }
 
 /**
  * Append a string to another string, reallocating as needed
  */
 char* string_append(char* original, const char* append) {
     if (!original) {
         return safe_strdup(append ? append : "");
     }
     
     if (!append) {
         return original;
     }
     
     size_t orig_len = strlen(original);
     size_t append_len = strlen(append);
     
     original = (char*)safe_realloc(original, orig_len + append_len + 1);
     strcat(original, append);
     
     return original;
 }