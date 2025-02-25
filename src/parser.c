/**
 * @file parser.c
 * @brief Implementation of parser functions
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "../include/parser.h"
 #include "../include/automaton.h"
 #include "../include/utils.h"
 
 // Forward declarations
 static void init_parser_stack(Parser* parser);
 static char* get_input_position_string(TokenStream* stream);
 
 Parser* parser_create(bool debug_mode) {
     Parser* parser = (Parser*)safe_malloc(sizeof(Parser));
     parser->stack = stack_create();
     parser->input = NULL;
     parser->tables = automaton_init();
     parser->debug_mode = debug_mode;
     parser->debug_file = NULL;
     parser->current_state = 0;
     parser->error_count = 0;
     
     // Initialize stack with initial state and EOF token
     init_parser_stack(parser);
     
     return parser;
 }
 
 void parser_free(Parser* parser) {
     if (!parser) {
         return;
     }
     
     stack_free(parser->stack);
     
     if (parser->debug_file) {
         fclose(parser->debug_file);
     }
     
     // Free parsing tables
     free_parsing_tables(parser->tables);
     
     // Input stream is closed separately
     
     free(parser);
 }
 
 ParseResult parser_parse(Parser* parser, const char* input_file, const char* output_file) {
     ParseResult result = {
         .success = false,
         .error_line = 0,
         .error_message = NULL,
         .debug_trace = NULL,
         .steps_taken = 0
     };
     
     if (!parser) {
         result.error_message = safe_strdup("Parser not initialized");
         return result;
     }
     
     // Open debug file if specified
     if (output_file && parser->debug_mode) {
         parser->debug_file = fopen(output_file, "w");
         if (!parser->debug_file) {
             result.error_message = string_format("Failed to open debug file: %s", output_file);
             return result;
         }
     }
     
     // Open input file
     parser->input = token_stream_create(input_file);
     if (!parser->input) {
         result.error_message = string_format("Failed to open input file: %s", input_file);
         return result;
     }
     
     // Main parsing loop
     int step = 0;
     bool done = false;
     
     while (!done && parser->input->current) {
         Token* current_token = parser->input->current;
         StackElement* top = stack_peek(parser->stack);
         
         if (!top) {
             result.error_message = safe_strdup("Stack underflow");
             break;
         }
         
         int state = top->state;
         int action_value = get_action(parser->tables, state, current_token->type);
         ActionType action_type = automaton_get_action_type(action_value);
         int action_param = automaton_get_action_value(action_value);
         
         step++;
         
         // Get debugging information
         char* stack_str = stack_to_string(parser->stack);
         char* input_pos = get_input_position_string(parser->input);
         char* action_str = automaton_action_to_string(action_value, parser->tables);
         
         switch (action_type) {
             case ACTION_SHIFT:
                 // Write debug output
                 write_debug_output(parser, "SHIFT", action_str);
                 
                 // Perform shift operation
                 if (!perform_shift(parser, action_param, current_token)) {
                     result.error_message = safe_strdup("Shift operation failed");
                     done = true;
                     break;
                 }
                 
                 // Get next token
                 get_next_token(parser->input);
                 break;
                 
             case ACTION_REDUCE:
                 // Write debug output
                 write_debug_output(parser, "REDUCE", action_str);
                 
                 // Perform reduce operation
                 if (!perform_reduce(parser, action_param)) {
                     result.error_message = safe_strdup("Reduce operation failed");
                     done = true;
                     break;
                 }
                 break;
                 
             case ACTION_ACCEPT:
                 // Write debug output
                 write_debug_output(parser, "ACCEPT", "Input accepted");
                 
                 result.success = true;
                 done = true;
                 break;
                 
             case ACTION_ERROR:
             default:
                 // Write debug output
                 write_debug_output(parser, "ERROR", "Invalid syntax");
                 
                 result.error_line = current_token->line_number;
                 result.error_message = string_format("Syntax error at line %d, position %d: unexpected token '%s'",
                                                    current_token->line_number,
                                                    current_token->position,
                                                    current_token->lexeme);
                 parser->error_count++;
                 done = true;
                 break;
         }
         
         free(stack_str);
         free(input_pos);
         free(action_str);
     }
     
     result.steps_taken = step;
     
     // Clean up token stream
     token_stream_free(parser->input);
     parser->input = NULL;
     
     return result;
 }
 
 void free_parsing_tables(ParsingTables* tables) {
     if (!tables) {
         return;
     }
     
     // Free action table
     for (int i = 0; i < tables->num_states; i++) {
         free(tables->action_table[i]);
     }
     free(tables->action_table);
     
     // Free goto table
     for (int i = 0; i < tables->num_states; i++) {
         free(tables->goto_table[i]);
     }
     free(tables->goto_table);
     
     // Free productions
     for (int i = 0; i <= tables->num_productions; i++) {
         free(tables->productions[i].rhs);
         free(tables->productions[i].rule_string);
     }
     free(tables->productions);
     
     free(tables);
 }
 
 bool perform_shift(Parser* parser, int state, Token* token) {
     if (!parser || !token) {
         return false;
     }
     
     // Push the current token and new state onto the stack
     bool result = stack_push(parser->stack, state, token);
     
     if (result) {
         parser->current_state = state;
     }
     
     return result;
 }
 
 bool perform_reduce(Parser* parser, int production_num) {
     if (!parser || !parser->tables || production_num <= 0 || production_num > parser->tables->num_productions) {
         return false;
     }
     
     Production* production = &parser->tables->productions[production_num];
     
     // Pop rhs_length symbols from the stack
     for (int i = 0; i < production->rhs_length; i++) {
         StackElement* element = stack_pop(parser->stack);
         if (!element) {
             log_error("Stack underflow during reduction");
             return false;
         }
         free(element);
     }
     
     // Get the state at the top of the stack
     StackElement* top = stack_peek(parser->stack);
     if (!top) {
         log_error("Stack underflow after reduction");
         return false;
     }
     
     int state = top->state;
     
     // Get the goto state for the LHS non-terminal
     int goto_state = get_goto_state(parser->tables, state, production->lhs);
     if (goto_state < 0) {
         log_error("Invalid goto state for non-terminal %d from state %d", production->lhs, state);
         return false;
     }
     
     // Create a dummy token for the LHS non-terminal
     // This is a simplified approach - in a real parser you might create AST nodes
     Token* lhs_token = token_create(production->lhs, "non-terminal", 0, 0);
     
     // Push the goto state and LHS non-terminal
     bool result = stack_push(parser->stack, goto_state, lhs_token);
     
     if (result) {
         parser->current_state = goto_state;
     }
     
     return result;
 }
 
 void write_debug_output(Parser* parser, const char* operation, const char* action) {
     if (!parser || !parser->debug_mode) {
         return;
     }
     
     static int step_number = 0;
     step_number++;
     
     char* stack_str = stack_to_string(parser->stack);
     char* input_pos = get_input_position_string(parser->input);
     
     // Debug output to file
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
 
 int get_action(ParsingTables* tables, int state, TokenType token_type) {
    if (!tables || state < 0 || state >= tables->num_states || 
        (int)token_type >= tables->num_terminals) {
        // Return error action
        return automaton_create_action(ACTION_ERROR, 0);
    }
    
    return tables->action_table[state][token_type];
}
 
 int get_goto_state(ParsingTables* tables, int state, int non_terminal) {
     if (!tables || state < 0 || state >= tables->num_states || 
         non_terminal < 0 || non_terminal >= tables->num_non_terminals) {
         return -1;
     }
     
     return tables->goto_table[state][non_terminal];
 }
 
 /* Internal function implementations */
 
 static void init_parser_stack(Parser* parser) {
     if (!parser || !parser->stack) {
         return;
     }
     
     // Create a dummy EOF token
     Token* eof_token = token_create(TOKEN_EOF, "$", 0, 0);
     
     // Push initial state and EOF token
     stack_push(parser->stack, 0, eof_token);
     parser->current_state = 0;
 }
 
 static char* get_input_position_string(TokenStream* stream) {
     if (!stream || !stream->current) {
         return safe_strdup("End of Input");
     }
     
     // Show current token and next few tokens
     char* result = safe_strdup("");
     Token* current = stream->current;
     int count = 0;
     
     while (current && count < 5) {
         char* token_str = token_to_string(current);
         
         // Add separator
         if (count > 0) {
             result = string_append(result, " ");
         }
         
         result = string_append(result, token_str);
         free(token_str);
         
         current = current->next;
         count++;
     }
     
     return result;
 }