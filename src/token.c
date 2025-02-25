/**
 * @file token.c
 * @brief Implementation of token functions
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
     "INVALID"
 };
 
 Token* token_create(TokenType type, const char* lexeme, int line, int position) {
     Token* token = (Token*)safe_malloc(sizeof(Token));
     token->type = type;
     token->lexeme = safe_strdup(lexeme);
     token->line_number = line;
     token->position = position;
     token->next = NULL;
     return token;
 }
 
 void token_free(Token* token) {
     if (token) {
         free(token->lexeme);
         free(token);
     }
 }
 
 const char* token_type_to_string(TokenType type) {
     if (type >= TOKEN_NUM && type <= TOKEN_INVALID) {
         return TOKEN_TYPE_NAMES[type];
     }
     return "UNKNOWN";
 }
 
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
     stream->head = stream->current;
     
     return stream;
 }
 
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
         }
     }
     
     return token;
 }
 
 bool has_more_tokens(TokenStream* stream) {
     return has_next_token(stream);
 }
 
 Token* peek_token(TokenStream* stream) {
     return peek_next_token(stream);
 }
 
 /**
  * Implementation of internal functions
  */
 
 static bool has_next_token(TokenStream* stream) {
     return stream && stream->current && stream->current->type != TOKEN_EOF;
 }
 
 static Token* peek_next_token(TokenStream* stream) {
     if (!stream || !stream->current) {
         return NULL;
     }
     return stream->current;
 }
 
 /**
  * Simple scanner that reads .cscn format tokens
  * This is a simplified implementation for demonstration
  */
 static Token* scan_token(TokenStream* stream) {
     if (!stream || !stream->input_file) {
         return NULL;
     }
     
     int c;
     static int line = 1;
     static int position = 0;
     
     // Skip whitespace
     while ((c = fgetc(stream->input_file)) != EOF) {
         if (c == '\n') {
             line++;
             position = 0;
         } else if (!isspace(c)) {
             position++;
             break;
         } else {
             position++;
         }
     }
     
     if (c == EOF) {
         return token_create(TOKEN_EOF, "EOF", line, position);
     }
     
     // Create token based on character
     TokenType type;
     char lexeme[2] = {c, '\0'};
     
     switch (c) {
         case '+':
             type = TOKEN_PLUS;
             break;
         case '*':
             type = TOKEN_STAR;
             break;
         case '(':
             type = TOKEN_LPAREN;
             break;
         case ')':
             type = TOKEN_RPAREN;
             break;
         default:
             // Check for numbers
             if (isdigit(c)) {
                 char number[32];
                 int i = 0;
                 number[i++] = c;
                 
                 // Read rest of number
                 while (i < 31 && (c = fgetc(stream->input_file)) != EOF && isdigit(c)) {
                     number[i++] = c;
                     position++;
                 }
                 
                 // Put back last character if not part of number
                 if (c != EOF && !isdigit(c)) {
                     ungetc(c, stream->input_file);
                 }
                 
                 number[i] = '\0';
                 return token_create(TOKEN_NUM, number, line, position - i + 1);
             } else {
                 // Invalid token
                 return token_create(TOKEN_INVALID, lexeme, line, position);
             }
     }
     
     return token_create(type, lexeme, line, position);
 }