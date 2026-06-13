#ifndef FLUX_TOKEN_H
#define FLUX_TOKEN_H

#include <string>

namespace Flux {

enum class TokenType {
    // Keywords
    T_FUNCTION, T_INT, T_FLOAT, T_STRING, T_BOOL,
    T_IF, T_ELSE, T_WHILE, T_RETURN,
    T_TRUE, T_FALSE,

    // Identifiers & Literals
    T_IDENTIFIER, T_INT_LITERAL, T_FLOAT_LITERAL, T_STRING_LITERAL,

    // Operators & Symbols
    T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
    T_COMMA, T_SEMICOLON, T_EQUALS,
    T_PLUS, T_MINUS, T_STAR, T_SLASH, T_PERCENT,
    T_PLUS_EQUAL, T_MINUS_EQUAL, T_STAR_EQUAL, T_SLASH_EQUAL,
    
    // Comparison
    T_EQUAL_EQUAL, T_BANG_EQUAL, T_BANG,
    T_GREATER, T_GREATER_EQUAL,
    T_LESS, T_LESS_EQUAL,

    // Logical
    T_AND_AND, T_OR_OR,

    T_EOF, T_INVALID
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(v), line(l), column(c) {}
};

} // namespace Flux

#endif
