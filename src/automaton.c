/**
 * @file automaton.c
 * @brief Implementation of shift/reduce automaton
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "../include/automaton.h"
 #include "../include/utils.h"
 
 // Constants for coding action values
 #define ACTION_SHIFT_BIT  0x10000
 #define ACTION_REDUCE_BIT 0x20000
 #define ACTION_ACCEPT_BIT 0x30000
 #define ACTION_ERROR_BIT  0x40000
 #define ACTION_MASK       0xF0000
 #define VALUE_MASK        0x0FFFF
 
 // Non-terminal symbols
 #define NON_TERMINAL_S 0
 #define NON_TERMINAL_E 1
 #define NON_TERMINAL_T 2
 #define NON_TERMINAL_F 3
 
 // Number of symbols and states
 #define NUM_TERMINALS      6  // NUM, PLUS, STAR, LPAREN, RPAREN, EOF
 #define NUM_NON_TERMINALS  4  // S, E, T, F
 #define NUM_STATES         11 // States 0-10
 #define NUM_PRODUCTIONS    7  // Productions 1-7
 
 // Grammar production rules from design document
 static Production grammar_productions[NUM_PRODUCTIONS + 1] = {
     {0, NULL, 0, NULL}, // Dummy production for 1-based indexing
     
     // 1. s → e
     {NON_TERMINAL_S, (int[]){NON_TERMINAL_E}, 1, "s → e"},
     
     // 2. e → e + t
     {NON_TERMINAL_E, (int[]){NON_TERMINAL_E, TOKEN_PLUS, NON_TERMINAL_T}, 3, "e → e + t"},
     
     // 3. e → t
     {NON_TERMINAL_E, (int[]){NON_TERMINAL_T}, 1, "e → t"},
     
     // 4. t → t * f
     {NON_TERMINAL_T, (int[]){NON_TERMINAL_T, TOKEN_STAR, NON_TERMINAL_F}, 3, "t → t * f"},
     
     // 5. t → f
     {NON_TERMINAL_T, (int[]){NON_TERMINAL_F}, 1, "t → f"},
     
     // 6. f → (e)
     {NON_TERMINAL_F, (int[]){TOKEN_LPAREN, NON_TERMINAL_E, TOKEN_RPAREN}, 3, "f → (e)"},
     
     // 7. f → NUM
     {NON_TERMINAL_F, (int[]){TOKEN_NUM}, 1, "f → NUM"}
 };
 
 // Forward declarations
 static void init_parsing_tables(ParsingTables* tables);
 static void setup_productions(ParsingTables* tables);
 
 ParsingTables* automaton_init() {
     ParsingTables* tables = (ParsingTables*)safe_malloc(sizeof(ParsingTables));
     
     tables->num_states = NUM_STATES;
     tables->num_terminals = NUM_TERMINALS;
     tables->num_non_terminals = NUM_NON_TERMINALS;
     tables->num_productions = NUM_PRODUCTIONS;
     
     // Allocate action table
     tables->action_table = (int**)safe_malloc(sizeof(int*) * NUM_STATES);
     for (int i = 0; i < NUM_STATES; i++) {
         tables->action_table[i] = (int*)safe_malloc(sizeof(int) * NUM_TERMINALS);
         // Initialize with error actions
         for (int j = 0; j < NUM_TERMINALS; j++) {
             tables->action_table[i][j] = automaton_create_action(ACTION_ERROR, 0);
         }
     }
     
     // Allocate goto table
     tables->goto_table = (int**)safe_malloc(sizeof(int*) * NUM_STATES);
     for (int i = 0; i < NUM_STATES; i++) {
         tables->goto_table[i] = (int*)safe_malloc(sizeof(int) * NUM_NON_TERMINALS);
         // Initialize with -1 (error)
         for (int j = 0; j < NUM_NON_TERMINALS; j++) {
             tables->goto_table[i][j] = -1;
         }
     }
     
     // Setup productions
     setup_productions(tables);
     
     // Initialize parsing tables based on the automaton
     init_parsing_tables(tables);
     
     return tables;
 }
 
 ActionType automaton_get_action_type(int action) {
     int type_bits = action & ACTION_MASK;
     
     switch (type_bits) {
         case ACTION_SHIFT_BIT:
             return ACTION_SHIFT;
         case ACTION_REDUCE_BIT:
             return ACTION_REDUCE;
         case ACTION_ACCEPT_BIT:
             return ACTION_ACCEPT;
         default:
             return ACTION_ERROR;
     }
 }
 
 int automaton_get_action_value(int action) {
     return action & VALUE_MASK;
 }
 
 int automaton_create_action(ActionType type, int value) {
     int action = 0;
     
     switch (type) {
         case ACTION_SHIFT:
             action = ACTION_SHIFT_BIT | (value & VALUE_MASK);
             break;
         case ACTION_REDUCE:
             action = ACTION_REDUCE_BIT | (value & VALUE_MASK);
             break;
         case ACTION_ACCEPT:
             action = ACTION_ACCEPT_BIT;
             break;
         case ACTION_ERROR:
         default:
             action = ACTION_ERROR_BIT;
             break;
     }
     
     return action;
 }
 
 char* automaton_action_to_string(int action_value, ParsingTables* tables) {
     ActionType type = automaton_get_action_type(action_value);
     int value = automaton_get_action_value(action_value);
     
     switch (type) {
         case ACTION_SHIFT:
             return string_format("Shift to state %d", value);
         case ACTION_REDUCE:
             if (value >= 1 && value <= tables->num_productions) {
                 return string_format("Reduce by rule %d: %s", 
                                     value, 
                                     tables->productions[value].rule_string);
             }
             return string_format("Reduce by unknown rule %d", value);
         case ACTION_ACCEPT:
             return safe_strdup("Accept");
         case ACTION_ERROR:
         default:
             return safe_strdup("Error");
     }
 }
 
 /* Internal function implementations */
 
 static void setup_productions(ParsingTables* tables) {
     // Allocate memory for productions
     tables->productions = (Production*)safe_malloc(sizeof(Production) * (NUM_PRODUCTIONS + 1));
     
     // Copy production rules
     for (int i = 0; i <= NUM_PRODUCTIONS; i++) {
         tables->productions[i].lhs = grammar_productions[i].lhs;
         tables->productions[i].rhs_length = grammar_productions[i].rhs_length;
         
         if (grammar_productions[i].rhs_length > 0) {
             tables->productions[i].rhs = (int*)safe_malloc(sizeof(int) * grammar_productions[i].rhs_length);
             memcpy(tables->productions[i].rhs, grammar_productions[i].rhs, 
                   sizeof(int) * grammar_productions[i].rhs_length);
         } else {
             tables->productions[i].rhs = NULL;
         }
         
         tables->productions[i].rule_string = safe_strdup(grammar_productions[i].rule_string ? 
                                                         grammar_productions[i].rule_string : "");
     }
 }
 
 static void init_parsing_tables(ParsingTables* tables) {
     // LR(0) parsing table for the grammar
     // This is a hardcoded version of the table based on the automaton in the design doc
     
     // State 0
     tables->goto_table[0][NON_TERMINAL_E] = 1;     // goto(0,E) = 1
     tables->goto_table[0][NON_TERMINAL_T] = 2;     // goto(0,T) = 2
     tables->goto_table[0][NON_TERMINAL_F] = 3;     // goto(0,F) = 3
     tables->action_table[0][TOKEN_NUM] = automaton_create_action(ACTION_SHIFT, 5);      // shift to state 5
     tables->action_table[0][TOKEN_LPAREN] = automaton_create_action(ACTION_SHIFT, 4);   // shift to state 4
     
     // State 1
     tables->action_table[1][TOKEN_PLUS] = automaton_create_action(ACTION_SHIFT, 6);     // shift to state 6
     tables->action_table[1][TOKEN_EOF] = automaton_create_action(ACTION_ACCEPT, 0);     // accept input
     
     // State 2
     tables->action_table[2][TOKEN_STAR] = automaton_create_action(ACTION_SHIFT, 7);     // shift to state 7
     tables->action_table[2][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 3);    // reduce using rule 3
     tables->action_table[2][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 3);  // reduce using rule 3
     tables->action_table[2][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 3);     // reduce using rule 3
     
     // State 3
     tables->action_table[3][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 5);    // reduce using rule 5
     tables->action_table[3][TOKEN_STAR] = automaton_create_action(ACTION_REDUCE, 5);    // reduce using rule 5
     tables->action_table[3][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 5);  // reduce using rule 5
     tables->action_table[3][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 5);     // reduce using rule 5
     
     // State 4
     tables->goto_table[4][NON_TERMINAL_E] = 8;     // goto(4,E) = 8
     tables->goto_table[4][NON_TERMINAL_T] = 2;     // goto(4,T) = 2
     tables->goto_table[4][NON_TERMINAL_F] = 3;     // goto(4,F) = 3
     tables->action_table[4][TOKEN_NUM] = automaton_create_action(ACTION_SHIFT, 5);      // shift to state 5
     tables->action_table[4][TOKEN_LPAREN] = automaton_create_action(ACTION_SHIFT, 4);   // shift to state 4
     
     // State 5
     tables->action_table[5][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 7);    // reduce using rule 7
     tables->action_table[5][TOKEN_STAR] = automaton_create_action(ACTION_REDUCE, 7);    // reduce using rule 7
     tables->action_table[5][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 7);  // reduce using rule 7
     tables->action_table[5][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 7);     // reduce using rule 7
     
     // State 6
     tables->goto_table[6][NON_TERMINAL_T] = 9;     // goto(6,T) = 9
     tables->goto_table[6][NON_TERMINAL_F] = 3;     // goto(6,F) = 3
     tables->action_table[6][TOKEN_NUM] = automaton_create_action(ACTION_SHIFT, 5);      // shift to state 5
     tables->action_table[6][TOKEN_LPAREN] = automaton_create_action(ACTION_SHIFT, 4);   // shift to state 4
     
     // State 7
     tables->goto_table[7][NON_TERMINAL_F] = 10;    // goto(7,F) = 10
     tables->action_table[7][TOKEN_NUM] = automaton_create_action(ACTION_SHIFT, 5);      // shift to state 5
     tables->action_table[7][TOKEN_LPAREN] = automaton_create_action(ACTION_SHIFT, 4);   // shift to state 4
     
     // State 8
     tables->action_table[8][TOKEN_PLUS] = automaton_create_action(ACTION_SHIFT, 6);     // shift to state 6
     tables->action_table[8][TOKEN_RPAREN] = automaton_create_action(ACTION_SHIFT, 11);  // shift to state 11
     
     // State 9
     tables->action_table[9][TOKEN_STAR] = automaton_create_action(ACTION_SHIFT, 7);     // shift to state 7
     tables->action_table[9][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 2);    // reduce using rule 2
     tables->action_table[9][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 2);  // reduce using rule 2
     tables->action_table[9][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 2);     // reduce using rule 2
     
     // State 10
     tables->action_table[10][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 4);   // reduce using rule 4
     tables->action_table[10][TOKEN_STAR] = automaton_create_action(ACTION_REDUCE, 4);   // reduce using rule 4
     tables->action_table[10][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 4); // reduce using rule 4
     tables->action_table[10][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 4);    // reduce using rule 4
     
     // State 11
     tables->action_table[11][TOKEN_PLUS] = automaton_create_action(ACTION_REDUCE, 6);   // reduce using rule 6
     tables->action_table[11][TOKEN_STAR] = automaton_create_action(ACTION_REDUCE, 6);   // reduce using rule 6
     tables->action_table[11][TOKEN_RPAREN] = automaton_create_action(ACTION_REDUCE, 6); // reduce using rule 6
     tables->action_table[11][TOKEN_EOF] = automaton_create_action(ACTION_REDUCE, 6);    // reduce using rule 6
 }