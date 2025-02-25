/**
 * @file parser.h
 * @brief Bottom-up parser implementation
 */

 #ifndef PARSER_H
 #define PARSER_H
 
 #include <stdio.h>
 #include <stdbool.h>
 #include "token.h"
 #include "stack.h"
 
 /**
  * @brief Grammar production rule
  */
 typedef struct {
     int lhs;            // Left-hand side non-terminal
     int* rhs;           // Right-hand side symbols
     int rhs_length;     // Length of right-hand side
     char* rule_string;  // String representation for debugging
 } Production;
 
 /**
  * @brief Parsing tables for LR parser
  */
 typedef struct {
     int** action_table;       // Action table [state][token]
     int** goto_table;         // Goto table [state][non-terminal]
     int num_states;           // Total number of states
     int num_terminals;        // Number of terminal symbols
     int num_non_terminals;    // Number of non-terminal symbols
     Production* productions;  // Array of production rules
     int num_productions;      // Number of productions
 } ParsingTables;
 
 /**
  * @brief Debug information during parsing
  */
 typedef struct {
     int step_number;       // Current parsing step
     char* operation;       // Current operation (shift/reduce)
     char* stack_contents;  // Current stack state
     char* input_position;  // Current input position
     char* action_taken;    // Details of action taken
 } DebugInfo;
 
 /**
  * @brief Parse result structure
  */
 typedef struct {
     bool success;           // Overall success/failure
     int error_line;         // Line number if error
     char* error_message;    // Error description
     DebugInfo* debug_trace; // Debug information
     int steps_taken;        // Number of parse steps
 } ParseResult;
 
 /**
  * @brief Parser structure
  */
 typedef struct {
     Stack* stack;          // Parser stack
     TokenStream* input;    // Input token stream
     ParsingTables* tables; // Action and goto tables
     bool debug_mode;       // Debug output control
     FILE* debug_file;      // Debug output file
     int current_state;     // Current parser state
     int error_count;       // Number of errors encountered
 } Parser;
 
 /**
  * @brief Initialize the parser
  * 
  * @param debug_mode Enable/disable debugging
  * @return Parser* Initialized parser or NULL on failure
  */
 Parser* parser_create(bool debug_mode);
 
 /**
  * @brief Clean up parser resources
  * 
  * @param parser Parser to free
  */
 void parser_free(Parser* parser);
 
 /**
  * @brief Parse input file
  * 
  * @param parser Initialized parser
  * @param input_file Input file path
  * @param output_file Output debug file path (can be NULL)
  * @return ParseResult Parse result
  */
 ParseResult parser_parse(Parser* parser, const char* input_file, const char* output_file);
 
 /**
  * @brief Create production rules for grammar
  * 
  * @return ParsingTables* Parsing tables or NULL on failure
  */
 ParsingTables* create_parsing_tables();
 
 /**
  * @brief Free parsing tables
  * 
  * @param tables Tables to free
  */
 void free_parsing_tables(ParsingTables* tables);
 
 /**
  * @brief Perform shift operation
  * 
  * @param parser Parser
  * @param state State to shift to
  * @param token Token to shift
  * @return bool True if successful
  */
 bool perform_shift(Parser* parser, int state, Token* token);
 
 /**
  * @brief Perform reduce operation
  * 
  * @param parser Parser
  * @param production_num Production rule number
  * @return bool True if successful
  */
 bool perform_reduce(Parser* parser, int production_num);
 
 /**
  * @brief Write debug information
  * 
  * @param parser Parser
  * @param operation Operation name
  * @param action Action details
  */
 void write_debug_output(Parser* parser, const char* operation, const char* action);
 
 /**
  * @brief Get action from parsing table
  * 
  * @param tables Parsing tables
  * @param state Current state
  * @param token_type Token type
  * @return int Action value
  */
 int get_action(ParsingTables* tables, int state, TokenType token_type);
 
 /**
  * @brief Get goto state from parsing table
  * 
  * @param tables Parsing tables
  * @param state Current state
  * @param non_terminal Non-terminal symbol
  * @return int Next state
  */
 int get_goto_state(ParsingTables* tables, int state, int non_terminal);
 
 #endif /* PARSER_H */