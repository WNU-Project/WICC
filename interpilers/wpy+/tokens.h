#ifndef TOKENS_H
#define TOKENS_H

// All possible token types in Python+
typedef enum {
    // Keywords
    TOKEN_FUNC,
    TOKEN_RETURN,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_USE,
    TOKEN_CONST,
    TOKEN_END,

    // Identifiers & literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQUAL,
    TOKEN_EQEQ,
    TOKEN_BANGEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_HASH,     // #
    TOKEN_INCLUDE,  // #include directive (optional)


    // Delimiters
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,

    // Special
    TOKEN_COMMENT_ONELINE,
    TOKEN_COMMENT_MULTILINE,
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *lexeme;   // actual text
    int line;       // line number in source
} Token;

#endif // TOKENS_H
