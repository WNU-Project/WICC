#ifndef TOKENS_H
#define TOKENS_H

// All possible token types in Python+
typedef enum {
    // Keywords
    TOKEN_FUNC,          // func
    TOKEN_RETURN,        // return
    TOKEN_IF,            // if
    TOKEN_ELSE,          // else
    TOKEN_FOR,           // for
    TOKEN_WHILE,         // while
    TOKEN_USE,           // use
    TOKEN_END,           // end
    // (Reserved for future: TOKEN_LET, TOKEN_CONST)

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

    // Namespaces / Sub-preprocessor directives
    TOKEN_VARIABLE,           // .variable
    TOKEN_TYPE_INT,           // int
    TOKEN_TYPE_CHAR,          // char
    TOKEN_CHAR_LITERAL,   // e.g. char.int(char1, 'A')
    TOKEN_TYPE_CHAR_STRING,   // string
    TOKEN_TYPE_FLOAT,         // float
    TOKEN_TYPE_BOOL,          // bool

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
