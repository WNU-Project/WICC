#ifndef TOKENS_H
#define TOKENS_H
// Token definitions for the WNU-C compiler (wnu-c)
// This header declares the TokenKind enum and a simple Token struct
// covering a broad C11 subset (identifiers, literals, keywords,
// operators and punctuation). It's intentionally conservative and
// can be extended as the lexer/parser are implemented.

typedef enum TokenKind {
	// Special
	TOK_UNKNOWN = 0,
	TOK_EOF,

	// Identifiers and literals
	TOK_IDENTIFIER,
	TOK_INT_LITERAL,
	TOK_FLOAT_LITERAL,
	TOK_CHAR_LITERAL,
	TOK_STRING_LITERAL,

	// Keywords (C11 subset)
	TOK_KW_AUTO,
	TOK_KW_BREAK,
	TOK_KW_CASE,
	TOK_KW_CHAR,
	TOK_KW_CONST,
	TOK_KW_CONTINUE,
	TOK_KW_DEFAULT,
	TOK_KW_DO,
	TOK_KW_DOUBLE,
	TOK_KW_ELSE,
	TOK_KW_ENUM,
	TOK_KW_EXTERN,
	TOK_KW_FLOAT,
	TOK_KW_FOR,
	TOK_KW_GOTO,
	TOK_KW_IF,
	TOK_KW_INLINE,
	TOK_KW_INT,
	TOK_KW_LONG,
	TOK_KW_REGISTER,
	TOK_KW_RESTRICT,
	TOK_KW_RETURN,
	TOK_KW_SHORT,
	TOK_KW_SIGNED,
	TOK_KW_SIZEOF,
	TOK_KW_STATIC,
	TOK_KW_STRUCT,
	TOK_KW_SWITCH,
	TOK_KW_TYPEDEF,
	TOK_KW_UNION,
	TOK_KW_UNSIGNED,
	TOK_KW_VOID,
	TOK_KW_VOLATILE,
	TOK_KW_WHILE,
	// C11/_Generic/_Noreturn/_Atomic and others
	TOK_KW__BOOL,
	TOK_KW__COMPLEX,
	TOK_KW__IMAGINARY,
	TOK_KW__ALIGNAS,
	TOK_KW__ALIGNOF,
	TOK_KW__ATOMIC,
	TOK_KW__GENERIC,
	TOK_KW__NORETURN,
	TOK_KW__STATIC_ASSERT,
	TOK_KW__THREAD_LOCAL,

	// Operators (symbolic tokens)
	TOK_PLUS,           // +
	TOK_MINUS,          // -
	TOK_STAR,           // *
	TOK_SLASH,          // /
	TOK_PERCENT,        // %
	TOK_PLUSPLUS,       // ++
	TOK_MINUSMINUS,     // --
	TOK_EQ,             // ==
	TOK_NEQ,            // !=
	TOK_LT,             // <
	TOK_GT,             // >
	TOK_LE,             // <=
	TOK_GE,             // >=
	TOK_ASSIGN,         // =
	TOK_PLUS_ASSIGN,    // +=
	TOK_MINUS_ASSIGN,   // -=
	TOK_MUL_ASSIGN,     // *=
	TOK_DIV_ASSIGN,     // /=
	TOK_MOD_ASSIGN,     // %=
	TOK_AND,            // &
	TOK_OR,             // |
	TOK_XOR,            // ^
	TOK_NOT,            // !
	TOK_LSHIFT,         // <<
	TOK_RSHIFT,         // >>
	TOK_AND_AND,        // &&
	TOK_OR_OR,          // ||
	TOK_AND_ASSIGN,     // &=
	TOK_OR_ASSIGN,      // |=
	TOK_XOR_ASSIGN,     // ^=
	TOK_LSHIFT_ASSIGN,  // <<=
	TOK_RSHIFT_ASSIGN,  // >>=
	TOK_TERNARY,        // ?
	TOK_COLON,          // :

	// Punctuation
	TOK_SEMICOLON,      // ;
	TOK_COMMA,          // ,
	TOK_DOT,            // .
	TOK_ARROW,          // ->
	TOK_LPAREN,         // (
	TOK_RPAREN,         // )
	TOK_LBRACE,         // {
	TOK_RBRACE,         // }
	TOK_LBRACKET,       // [
	TOK_RBRACKET,       // ]

	// Preprocessor tokens (kept simple here)
	TOK_PP_HASH,        // #
	TOK_PP_HASHHASH     // ##
} TokenKind;

// A simple token representation for the front-end. Lexer will allocate
// lexeme strings (or point into a buffer) and set location info.
typedef struct Token {
	TokenKind kind;
	const char *lexeme; // pointer into source buffer or NUL-terminated copy
	int line;
	int column;
} Token;

#endif // TOKENS_H