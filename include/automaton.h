/**
 * @file automaton.h
 * @brief Shift/reduce automaton for bottom-up parser
 */

 #ifndef AUTOMATON_H
 #define AUTOMATON_H
 
 #include "parser.h"
 
 /**
  * @brief Automaton action types
  */
 typedef enum {
     ACTION_SHIFT,   // Shift action
     ACTION_REDUCE,  // Reduce action
     ACTION_ACCEPT,  // Accept input
     ACTION_ERROR    // Parsing error
 } ActionType;
 
 /**
  * @brief Initialize automaton with grammar rules
  * 
  * @return ParsingTables* Created parsing tables
  */
 ParsingTables* automaton_init();
 
 /**
  * @brief Decode action from action table entry
  * 
  * @param action Action value from table
  * @return ActionType Decoded action type
  */
 ActionType automaton_get_action_type(int action);
 
 /**
  * @brief Get value associated with action
  * 
  * @param action Action value from table
  * @return int State for shift, production for reduce
  */
 int automaton_get_action_value(int action);
 
 /**
  * @brief Create action value for table
  * 
  * @param type Action type
  * @param value State or production number
  * @return int Encoded action value
  */
 int automaton_create_action(ActionType type, int value);
 
 /**
  * @brief Get string representation of automaton action
  * 
  * @param action_value Action value from table
  * @param tables Parsing tables for production lookup
  * @return char* String representation (must be freed by caller)
  */
 char* automaton_action_to_string(int action_value, ParsingTables* tables);
 
 #endif /* AUTOMATON_H */