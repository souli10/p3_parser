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

 /**
  * Write debugging information to output file/console
  */
 void write_debug_output(Parser* parser, const char* operation, const char* action) {
    static int step_number = 0;
    step_number++;
    
    char* stack_str = stack_to_string(parser->stack);
    char* input_pos = get_input_position_string(parser->input);
    
    // Always print to console regardless of debug_mode
    printf("Step %d:\n", step_number);
    printf("Current State: %d\n", parser->current_state);
    printf("Stack Contents: %s\n", stack_str);
    printf("Input Position: %s\n", input_pos);
    printf("Operation: %s\n", operation);
    printf("Action: %s\n\n", action);
    
     
     // Debug output to file (format as specified in the design document)
     if (parser->debug_file) {
         fprintf(parser->debug_file, "Step %d:\n", step_number);
         fprintf(parser->debug_file, "Current State: %d\n", parser->current_state);
         fprintf(parser->debug_file, "Stack Contents: %s\n", stack_str);
         fprintf(parser->debug_file, "Input Position: %s\n", input_pos);
         fprintf(parser->debug_file, "Operation: %s\n", operation);
         fprintf(parser->debug_file, "Action: %s\n\n", action);
     }
     
     // Debug output to console if enabled
     if (parser->debug_mode) {
         log_debug(true, "Step %d:", step_number);
         log_debug(true, "Current State: %d", parser->current_state);
         log_debug(true, "Stack Contents: %s", stack_str);
         log_debug(true, "Input Position: %s", input_pos);
         log_debug(true, "Operation: %s", operation);
         log_debug(true, "Action: %s", action);
         log_debug(true, "--------------------");
     }
     
     free(stack_str);
     free(input_pos);
 }

  /**
  * Print program usage information
  */
 void print_usage(const char* program_name) {
    printf("Usage: %s <input_file>\n", program_name);
    printf("  <input_file>: Path to the input file (.cscn)\n");
    printf("  Output will be saved to <input_file>_p3dbg.txt\n");
}

/**
 * Generate output filename from input filename
 * Follows the format: <input_basename>_p3dbg.txt
 */
char* generate_output_filename(const char* input_file) {
    // Extract the base filename without path
    char* base_name = strrchr(input_file, '/');
    if (base_name == NULL) {
        base_name = strrchr(input_file, '\\');
    }
    
    if (base_name != NULL) {
        base_name++; // Skip the slash
    } else {
        base_name = (char*)input_file; // No path separator found
    }
    
    // Find extension and remove it if present
    char* basename_copy = safe_strdup(base_name);
    char* dot = strrchr(basename_copy, '.');
    if (dot != NULL) {
        *dot = '\0'; // Truncate at the dot
    }
    
    // Create output filename: <basename>_p3dbg.txt
    char* output_file = string_format("%s_p3dbg.txt", basename_copy);
    free(basename_copy);
    
    return output_file;
}