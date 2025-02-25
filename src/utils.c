/**
 * @file utils.c
 * @brief Implementation of utility functions
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdarg.h>
 #include "../include/utils.h"
 
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
 
 void* safe_malloc(size_t size) {
     void* ptr = malloc(size);
     if (!ptr) {
         log_error("Memory allocation failed (malloc)");
         exit(EXIT_FAILURE);
     }
     
     return ptr;
 }
 
 void* safe_realloc(void* ptr, size_t size) {
     void* new_ptr = realloc(ptr, size);
     if (!new_ptr) {
         log_error("Memory allocation failed (realloc)");
         exit(EXIT_FAILURE);
     }
     
     return new_ptr;
 }
 
 void log_error(const char* format, ...) {
     va_list args;
     va_start(args, format);
     
     fprintf(stderr, "ERROR: ");
     vfprintf(stderr, format, args);
     fprintf(stderr, "\n");
     
     va_end(args);
 }
 
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