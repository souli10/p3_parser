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