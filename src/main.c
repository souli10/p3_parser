/**
 * @file main.c
 * @brief Main entry point for bottom-up parser
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 #include "../include/parser.h"
 #include "../include/utils.h"
 
 void print_usage(const char* program_name) {
     printf("Usage: %s <input_file>\n", program_name);
     printf("  <input_file>: Path to the input file (.cscn)\n");
     printf("  Output will be saved to parsed_<input_file>\n");
 }
 
 // Function to generate output filename
 char* generate_output_filename(const char* input_file) {
     char* base_name = strrchr(input_file, '/');
     if (base_name == NULL) {
         base_name = strrchr(input_file, '\\');
     }
     
     if (base_name != NULL) {
         base_name++; // Skip the slash
     } else {
         base_name = (char*)input_file; // No path separator found
     }
     
     // Allocate memory for "parsed_" + base_name + null terminator
     char* output_file = (char*)safe_malloc(strlen("parsed_") + strlen(base_name) + 1);
     
     strcpy(output_file, "parsed_");
     strcat(output_file, base_name);
     
     return output_file;
 }
 
 int main(int argc, char* argv[]) {
     if (argc != 2) {
         print_usage(argv[0]);
         return EXIT_FAILURE;
     }

     const char* input_file = argv[1];
     char* output_file = generate_output_filename(input_file);
     bool debug_mode = true; // Always enable debug mode
     
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