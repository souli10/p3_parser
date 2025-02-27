/**
 * @file main.c
 * @brief Main entry point for bottom-up parser
 * @members: Group 
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include "../include/parser.h"
 #include "../include/utils.h"
 
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
 
 /**
  * Main function
  */
 int main(int argc, char* argv[]) {

     if (argc != 2) {
         print_usage(argv[0]);
         return EXIT_FAILURE;
     }
  
     const char* input_file = argv[1];
     char* output_file = generate_output_filename(input_file);

     bool debug_mode = true; // Always enable debug mode as per the design document

     // Create parser
     Parser* parser = parser_create(debug_mode);
   
     if (!parser) {
         fprintf(stderr, "Error: Failed to create parser\n");
         free(output_file);
         return EXIT_FAILURE;
     }
     
     printf("Starting parser...\n");
     printf("Input file: %s\n", input_file);
     printf("Output file: %s\n", output_file);
     
     // Parse input
     ParseResult result = parser_parse(parser, input_file, output_file);
     
     // Print result
     if (result.success) {
         printf("\nParsing completed successfully.\n");
         printf("Steps taken: %d\n", result.steps_taken);
         printf("Output saved to %s\n", output_file);
     } else {
         fprintf(stderr, "\nParsing failed!\n");
         if (result.error_message) {
             fprintf(stderr, "Error: %s\n", result.error_message);
         }
         if (result.error_line > 0) {
             fprintf(stderr, "Error occurred at line %d\n", result.error_line);
         }
     }
     
     // Clean up
     if (result.error_message) {
         free(result.error_message);
     }
     if (result.debug_trace) {
         free(result.debug_trace);
     }
     
     parser_free(parser);
     free(output_file);
     
     return result.success ? EXIT_SUCCESS : EXIT_FAILURE;
 }