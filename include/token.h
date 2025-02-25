/**
 * @file token.h
 * @brief Token representation and management for bottom-up parser
 */

 #ifndef TOKEN_H
 #define TOKEN_H
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 
 /**
  * @brief Token types for the parser
  */
 typedef enum {
     TOKEN_NUM,    // Number
     TOKEN_PLUS,   // '+'
     TOKEN_STAR,   // '*'
     TOKEN_LPAREN, // '('
     TOKEN_RPAREN, // ')'
     TOKEN_EOF,    // End of file
     TOKEN_INVALID // Invalid token
 } TokenType;
 
 /**
  * @brief Token structure for lexical analysis
  */
 typedef struct Token {
     TokenType type;      // Token type
     char* lexeme;        // Actual string content of token
     int line_number;     // Source code line number
     int position;        // Position in line
     struct Token* next;  // For token list management
 } Token;
 
 /**
  * @brief TokenStream structure for token iteration
  */
 typedef struct TokenStream {
     Token* current;     // Current token being processed
     Token* head;        // Start of token list
     FILE* input_file;   // Source file
     int token_count;    // Total tokens processed
     bool (*has_next)(struct TokenStream*);   // Function to check if more tokens exist
     Token* (*peek_next)(struct TokenStream*); // Function to peek at next token
 } TokenStream;
 
 /**
  * @brief Create a new token
  * 
  * @param type Token type
  * @param lexeme Token content
  * @param line Line number
  * @param position Position in line
  * @return Token* New token or NULL if allocation fails
  */
 Token* token_create(TokenType type, const char* lexeme, int line, int position);
 
 /**
  * @brief Free token memory
  * 
  * @param token Token to free
  */
 void token_free(Token* token);
 
 /**
  * @brief Convert token type to string representation
  * 
  * @param type Token type
  * @return const char* String representation
  */
 const char* token_type_to_string(TokenType type);
 
 /**
  * @brief Get string representation of a token
  * 
  * @param token Token
  * @return char* String representation (must be freed by caller)
  */
 char* token_to_string(Token* token);
 
 /**
  * @brief Initialize a token stream from an input file
  * 
  * @param filename Input file name
  * @return TokenStream* Initialized token stream or NULL on failure
  */
 TokenStream* token_stream_create(const char* filename);
 
 /**
  * @brief Free a token stream and all its tokens
  * 
  * @param stream Token stream to free
  */
 void token_stream_free(TokenStream* stream);
 
 /**
  * @brief Get next token from stream
  * 
  * @param stream Token stream
  * @return Token* Next token or NULL if no more tokens
  */
 Token* get_next_token(TokenStream* stream);
 
 /**
  * @brief Check if token stream has more tokens
  * 
  * @param stream Token stream
  * @return true If more tokens exist
  * @return false If end of stream
  */
 bool has_more_tokens(TokenStream* stream);
 
 /**
  * @brief Peek at next token without advancing
  * 
  * @param stream Token stream
  * @return Token* Next token or NULL if no more tokens
  */
 Token* peek_token(TokenStream* stream);
 
 #endif /* TOKEN_H */