/**
 * @file stack.h
 * @brief Stack implementation for bottom-up parser
 */

 #ifndef STACK_H
 #define STACK_H
 
 #include <stdbool.h>
 #include "token.h"
 
 /**
  * @brief Stack element structure for parsing
  */
 typedef struct StackElement {
     int state;                  // Current parsing state
     Token* symbol;              // Current symbol
     struct StackElement* next;  // Next stack element
 } StackElement;
 
 /**
  * @brief Stack structure for parser
  */
 typedef struct {
     StackElement* top;  // Top of the stack
     int size;           // Current stack size
 } Stack;
 
 /**
  * @brief Create a new stack
  * 
  * @return Stack* New empty stack or NULL on failure
  */
 Stack* stack_create();
 
 /**
  * @brief Free stack and all its elements
  * 
  * @param stack Stack to free
  */
 void stack_free(Stack* stack);
 
 /**
  * @brief Push state and symbol onto stack
  * 
  * @param stack Stack
  * @param state State to push
  * @param symbol Symbol to push (will be copied)
  * @return true If push successful
  * @return false If push failed
  */
 bool stack_push(Stack* stack, int state, Token* symbol);
 
 /**
  * @brief Pop top element from stack
  * 
  * @param stack Stack
  * @return StackElement* Popped element (caller must free) or NULL if stack empty
  */
 StackElement* stack_pop(Stack* stack);
 
 /**
  * @brief Peek at top element without removing
  * 
  * @param stack Stack
  * @return StackElement* Top element or NULL if stack empty
  */
 StackElement* stack_peek(Stack* stack);
 
 /**
  * @brief Check if stack is empty
  * 
  * @param stack Stack
  * @return true If stack is empty
  * @return false If stack has elements
  */
 bool stack_is_empty(Stack* stack);
 
 /**
  * @brief Get stack size
  * 
  * @param stack Stack
  * @return int Number of elements in stack
  */
 int stack_size(Stack* stack);
 
 /**
  * @brief Get string representation of stack contents
  * 
  * @param stack Stack
  * @return char* String representation (must be freed by caller)
  */
 char* stack_to_string(Stack* stack);
 
 #endif /* STACK_H */