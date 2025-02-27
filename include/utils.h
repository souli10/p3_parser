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

  /**
  * @brief Write debug information
  * 
  * @param parser Parser
  * @param operation Operation name
  * @param action Action details
  */
 void write_debug_output(Parser* parser, const char* operation, const char* action);

/**
 * @brief prints the correct usage in the terminal
 * 
 * @param program_name The program name 
 */
 void print_usage(const char* program_name);

 /**
  * @brief Generates the name of the output file
  * 
  * @param input_file The input file of the program
  */
 char* generate_output_filename(const char* input_file);
 
 #endif /* UTILS_H */