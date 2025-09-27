#ifndef TOKENS_H
#define TOKENS_H

// All possible token types in Python++
typedef enum {
    // Keywords
    TOKEN_FUNC,
    TOKEN_RETURN,
    TOKEN_SUCCESS,
    TOKEN_FAILURE,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_INCLUDE,

    // Identifiers and literals
    TOKEN_IDENTIFIER,
    TOKEN_INT_LITERAL,
    TOKEN_CHAR_LITERAL,
    TOKEN_STRING_LITERAL,

    // Symbols
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_SEMICOLON,  // ;
    TOKEN_COMMA,      // ,
    TOKEN_DOT,        // .

    // Operators
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_STAR,       // *
    TOKEN_SLASH,      // /
    TOKEN_ASSIGN,     // =
    TOKEN_EQ,         // ==
    TOKEN_GT,         // >
    TOKEN_LT,         // <
    TOKEN_NEQ,        // !=
    TOKEN_GTE,        // >=
    TOKEN_LTE,        // <=

    // End of file
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;   // kind of token
    char *lexeme;     // actual text
    int line;         // line number
    int column;       // column number
} Token;

// Lexer API
void init_lexer(const char *source);
Token next_token(void);
void free_token(Token *tok);

#endif // TOKENS_H
