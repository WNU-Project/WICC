#ifndef TOKENS_H
#define TOKENS_H

// All possible token types in Python+
typedef enum {
    // Keywords
    TOKEN_FUNC,          // func
    TOKEN_RETURN,        // return
    TOKEN_LET,           // let
    TOKEN_IF,            // if
    TOKEN_ELSE,          // else
    TOKEN_FOR,           // for
    TOKEN_WHILE,         // while
    TOKEN_USE,           // use
    TOKEN_CONST,         // const
    TOKEN_END,           // end

    // Identifiers & literals
    TOKEN_IDENTIFIER,    // variable/function names
    TOKEN_NUMBER,        // numeric literal
    TOKEN_STRING,        // string literal

    // Operators
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_STAR,          // *
    TOKEN_SLASH,         // /
    TOKEN_EQUAL,         // =
    TOKEN_EQEQ,          // ==
    TOKEN_BANGEQ,        // !=
    TOKEN_LT,            // <
    TOKEN_GT,            // >
    TOKEN_HASH,          // #
    TOKEN_INCLUDE,       // #include <...>

    // Delimiters
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_SEMICOLON,     // ;
    TOKEN_COMMA,         // ,
    TOKEN_DOT,           // .

    // Special
    TOKEN_COMMENT_ONELINE,   // //...
    TOKEN_COMMENT_MULTILINE, // /* ... */
    TOKEN_EOF                // end of file
} TokenType;

// Token structure
typedef struct {
    TokenType type;   // kind of token
    char *lexeme;     // actual text (heap-allocated copy)
    int line;         // line number in source
} Token;

#endif // TOKENS_H
