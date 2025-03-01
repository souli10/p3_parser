/**
 * token.c
 * Fixed implementation that handles multiple tokens per line
 * and properly represents non-terminal symbols
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <ctype.h>
 #include "../include/token.h"
 #include "../include/utils.h"
 
 /**
  * Internal function declarations
  */
 static bool has_next_token(TokenStream* stream);
 static Token* peek_next_token(TokenStream* stream);
 static Token* scan_token(TokenStream* stream);
 
 /**
  * Token type name mapping
  */
 static const char* TOKEN_TYPE_NAMES[] = {
     "NUM",
     "PLUS",
     "STAR",
     "LPAREN",
     "RPAREN",
     "EOF",
     "INVALID",
     "NON_TERMINAL"
 };
 
 /**
  * Non-terminal symbol names
  */
 static const char* NON_TERMINAL_NAMES[] = {
     "S",  // Start symbol
     "E",  // Expression
     "T",  // Term
     "F"   // Factor
 };
 
 /**
  * @brief Get string name of a non-terminal symbol
  */
 const char* get_non_terminal_name(int non_terminal) {
    if (non_terminal >= 0 && non_terminal < (int)(sizeof(NON_TERMINAL_NAMES)/sizeof(NON_TERMINAL_NAMES[0]))) {
         return NON_TERMINAL_NAMES[non_terminal];
     }
     return "UNKNOWN";
 }
 
 /**
  * Convert token type string to enum
  */
 TokenType token_type_from_string(const char* type_str) {
     for (int i = 0; i <= TOKEN_NON_TERMINAL; i++) {
         if (strcmp(type_str, TOKEN_TYPE_NAMES[i]) == 0) {
             return (TokenType)i;
         }
     }
     return TOKEN_INVALID;
 }
 
 /**
  * Create a new token
  */
 Token* token_create(TokenType type, const char* lexeme, int line, int position) {
     Token* token = (Token*)safe_malloc(sizeof(Token));
     token->type = type;
     token->lexeme = safe_strdup(lexeme);
     token->line_number = line;
     token->position = position;
     token->next = NULL;
     return token;
 }
 
 /**
  * Free token memory
  */
 void token_free(Token* token) {
     if (token) {
         free(token->lexeme);
         free(token);
     }
 }
 
 /**
  * Convert token type to string representation
  */
 const char* token_type_to_string(TokenType type) {
     if (type >= TOKEN_NUM && type <= TOKEN_NON_TERMINAL) {
         return TOKEN_TYPE_NAMES[type];
     }
     return "UNKNOWN";
 }
 
 /**
  * Convert token to string representation
  */
 char* token_to_string(Token* token) {
     if (!token) {
         return safe_strdup("NULL");
     }
     
     return string_format("<%s, \"%s\", %d, %d>", 
                          token_type_to_string(token->type),
                          token->lexeme,
                          token->line_number,
                          token->position);
 }
 
 /**
  * Create a token stream from a file
  */
 TokenStream* token_stream_create(const char* filename) {
     FILE* file = fopen(filename, "r");
     if (!file) {
         log_error("Failed to open file: %s", filename);
         return NULL;
     }
     
     TokenStream* stream = (TokenStream*)safe_malloc(sizeof(TokenStream));
     stream->input_file = file;
     stream->head = NULL;
     stream->current = NULL;
     stream->token_count = 0;
     stream->has_next = has_next_token;
     stream->peek_next = peek_next_token;
     
     // Read the first token
     stream->current = scan_token(stream);
     if (stream->current) {
         printf("First token: %s, type: %s\n", 
                stream->current->lexeme, 
                token_type_to_string(stream->current->type));
     } else {
         printf("No tokens read from file\n");
     }
     
     stream->head = stream->current;
     
     return stream;
 }
 
 /**
  * Free token stream and all its tokens
  */
 void token_stream_free(TokenStream* stream) {
     if (!stream) {
         return;
     }
     
     // Free all tokens
     Token* current = stream->head;
     while (current) {
         Token* next = current->next;
         token_free(current);
         current = next;
     }
     
     // Close file
     if (stream->input_file) {
         fclose(stream->input_file);
     }
     
     free(stream);
 }
 
 /**
  * Get the next token from the stream
  */
 Token* get_next_token(TokenStream* stream) {
     if (!stream || !stream->current) {
         return NULL;
     }
     
     Token* token = stream->current;
     
     // Advance to next token
     if (stream->current->type != TOKEN_EOF) {
         if (stream->current->next) {
             stream->current = stream->current->next;
         } else {
             // Read next token from file
             Token* next_token = scan_token(stream);
             stream->current->next = next_token;
             stream->current = next_token;
             
             if (next_token) {
                 printf("Next token: %s, type: %s\n", 
                        next_token->lexeme, 
                        token_type_to_string(next_token->type));
             }
         }
     }
     
     return token;
 }
 
 /**
  * Check if stream has more tokens
  */
 bool has_more_tokens(TokenStream* stream) {
     return has_next_token(stream);
 }
 
 /**
  * Peek at the next token without advancing
  */
 Token* peek_token(TokenStream* stream) {
     return peek_next_token(stream);
 }
 
 /**
  * Implementation of internal functions
  */
 
 /**
  * Check if stream has a next token
  */
 static bool has_next_token(TokenStream* stream) {
     return stream && stream->current && stream->current->type != TOKEN_EOF;
 }
 
 /**
  * Peek at the next token without advancing
  */
 static Token* peek_next_token(TokenStream* stream) {
     if (!stream || !stream->current) {
         return NULL;
     }
     return stream->current;
 }
 
 /**
  * Parse tokens from file
  * Handles multiple tokens per line and properly formats them
  */
 static Token* scan_token(TokenStream* stream) {
     if (!stream || !stream->input_file) {
         return NULL;
     }
     
     static int line = 1;
     static int position = 0;
     static char buffer[1024] = "";
     static char* current_pos = NULL;
     
     // Initialize or read a new line if needed
     if (buffer[0] == '\0' || current_pos == NULL || *current_pos == '\0') {
         if (fgets(buffer, sizeof(buffer), stream->input_file) == NULL) {
             printf("End of file reached\n");
             return token_create(TOKEN_EOF, "EOF", line, position);
         }
         
         printf("Read line: '%s'\n", buffer);
         current_pos = buffer;
         position = 0;
     }
     
     // Skip whitespace
     while (*current_pos && isspace((unsigned char)*current_pos)) {
         if (*current_pos == '\n') {
             line++;
             position = 0;
         } else {
             position++;
         }
         current_pos++;
     }
     
     // If end of line, reset for next line read
     if (*current_pos == '\0') {
         buffer[0] = '\0';
         return scan_token(stream);
     }
     
     // Check if token starts with '<'
     if (*current_pos == '<') {
         char* token_start = current_pos;
         char* token_end = strchr(current_pos, '>');
         
         if (token_end) {
             // Extract token text
             int token_len = token_end - token_start + 1;
             char* token_text = (char*)safe_malloc(token_len + 1);
             strncpy(token_text, token_start, token_len);
             token_text[token_len] = '\0';
             
             printf("Processing token: '%s'\n", token_text);
             
             // Parse token components
             char lexeme[128] = "";
             char category[128] = "";
             
             if (sscanf(token_text, "<%[^,], %[^>]>", lexeme, category) == 2) {
                 printf("Parsed token: lexeme='%s', category='%s'\n", lexeme, category);
                 
                 // Cleanup
                 free(token_text);
                 
                 // Convert category string to token type
                 TokenType type = token_type_from_string(category);
                 
                 // Update position for next call
                 int token_position = position;
                 position += token_len;
                 current_pos = token_end + 1;
                 
                 // Create token
                 return token_create(type, lexeme, line, token_position);
             }
             else if (sscanf(token_text, "<%[^,],%[^>]>", lexeme, category) == 2) {
                 // Try alternative format with no space after comma
                 printf("Parsed token (alt format): lexeme='%s', category='%s'\n", lexeme, category);
                 
                 // Cleanup
                 free(token_text);
                 
                 // Convert category string to token type
                 TokenType type = token_type_from_string(category);
                 
                 // Update position for next call
                 int token_position = position;
                 position += token_len;
                 current_pos = token_end + 1;
                 
                 // Create token
                 return token_create(type, lexeme, line, token_position);
             }
             
             free(token_text);
         }
     }
     
     // If we get here, we couldn't parse a token at the current position
     printf("Invalid token format at line %d, position %d: %.10s...\n", 
            line, position, current_pos);
     
     // Skip to the next '<' or end of line
     while (*current_pos && *current_pos != '<' && *current_pos != '\n') {
         current_pos++;
         position++;
     }
     
     // If we hit a newline, handle it
     if (*current_pos == '\n') {
         line++;
         position = 0;
         current_pos++;
         
         // If at end of buffer, reset for next line read
         if (*current_pos == '\0') {
             buffer[0] = '\0';
         }
     }
     
     // Try again recursively
     return scan_token(stream);
 }