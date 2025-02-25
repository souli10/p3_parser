/**
 * @file stack.c
 * @brief Implementation of stack functions
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "../include/stack.h"
 #include "../include/utils.h"
 
 Stack* stack_create() {
     Stack* stack = (Stack*)safe_malloc(sizeof(Stack));
     stack->top = NULL;
     stack->size = 0;
     return stack;
 }
 
 void stack_free(Stack* stack) {
     if (!stack) {
         return;
     }
     
     // Free all elements
     while (!stack_is_empty(stack)) {
         StackElement* element = stack_pop(stack);
         // Don't free the token as it belongs to the token stream
         free(element);
     }
     
     free(stack);
 }
 
 bool stack_push(Stack* stack, int state, Token* symbol) {
     if (!stack) {
         return false;
     }
     
     StackElement* element = (StackElement*)safe_malloc(sizeof(StackElement));
     element->state = state;
     element->symbol = symbol; // Just store a reference, don't copy
     element->next = stack->top;
     
     stack->top = element;
     stack->size++;
     
     return true;
 }
 
 StackElement* stack_pop(Stack* stack) {
     if (!stack || stack_is_empty(stack)) {
         return NULL;
     }
     
     StackElement* element = stack->top;
     stack->top = element->next;
     stack->size--;
     
     element->next = NULL;
     return element;
 }
 
 StackElement* stack_peek(Stack* stack) {
     if (!stack || stack_is_empty(stack)) {
         return NULL;
     }
     
     return stack->top;
 }
 
 bool stack_is_empty(Stack* stack) {
     return !stack || stack->top == NULL;
 }
 
 int stack_size(Stack* stack) {
     return stack ? stack->size : 0;
 }
 
 char* stack_to_string(Stack* stack) {
     if (!stack) {
         return safe_strdup("[]");
     }
     
     char* result = safe_strdup("[");
     StackElement* current = stack->top;
     
     // Build string from top to bottom
     StackElement** elements = (StackElement**)safe_malloc(sizeof(StackElement*) * stack->size);
     int count = 0;
     
     while (current) {
         elements[count++] = current;
         current = current->next;
     }
     
     // Append elements in reverse order
     for (int i = count - 1; i >= 0; i--) {
         char* token_str = token_to_string(elements[i]->symbol);
         char* element_str = string_format("[%d %s]", elements[i]->state, token_str);
         
         if (i < count - 1) {
             result = string_append(result, " ");
         }
         
         result = string_append(result, element_str);
         
         free(token_str);
         free(element_str);
     }
     
     result = string_append(result, "]");
     free(elements);
     
     return result;
 }