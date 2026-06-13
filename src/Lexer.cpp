#include "Lexer.h"
#include <cctype>

namespace Flux {

const std::map<std::string, TokenType> Lexer::keywords = {
    {"function", TokenType::T_FUNCTION},
    {"func", TokenType::T_FUNCTION},
    {"int", TokenType::T_INT},
    {"float", TokenType::T_FLOAT},
    {"string", TokenType::T_STRING},
    {"bool", TokenType::T_BOOL},
    {"if", TokenType::T_IF},
    {"else", TokenType::T_ELSE},
    {"while", TokenType::T_WHILE},
    {"return", TokenType::T_RETURN},
    {"import", TokenType::T_IMPORT},
    {"true", TokenType::T_TRUE},
    {"false", TokenType::T_FALSE},
    {"and", TokenType::T_AND_AND},
    {"or", TokenType::T_OR_OR},
    {"not", TokenType::T_BANG}
};

Lexer::Lexer(const std::string& source) : source(source), pos(0), line(1), column(1) {}

char Lexer::peek() const { return isAtEnd() ? '\0' : source[pos]; }
char Lexer::peekNext() const { return (pos + 1 >= source.length()) ? '\0' : source[pos+1]; }

char Lexer::advance() {
    char current = source[pos++];
    if (current == '\n') { line++; column = 1; }
    else { column++; }
    return current;
}

bool Lexer::isAtEnd() const { return pos >= source.length(); }

void Lexer::skipWhitespaceAndComments() {
    // Skip UTF-8 BOM
    if (pos == 0 && source.size() >= 3 && 
        (unsigned char)source[0] == 0xEF && 
        (unsigned char)source[1] == 0xBB && 
        (unsigned char)source[2] == 0xBF) {
        pos = 3; column = 4;
    }
    while (!isAtEnd()) {
        unsigned char c = static_cast<unsigned char>(peek());
        if (std::isspace(c)) advance();
        else if (c == '/' && peekNext() == '/') {
            while (!isAtEnd() && peek() != '\n') advance();
        } else break;
    }
}

Token Lexer::readIdentifier() {
    int sc = column;
    std::string val;
    while (!isAtEnd()) {
        unsigned char c = static_cast<unsigned char>(peek());
        if (std::isalnum(c) || c == '_') val += advance();
        else break;
    }
    if (keywords.count(val)) return Token(keywords.at(val), val, line, sc);
    return Token(TokenType::T_IDENTIFIER, val, line, sc);
}

Token Lexer::readNumber() {
    int sc = column;
    std::string val;
    bool isF = false;
    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) val += advance();
    if (peek() == '.') {
        isF = true; val += advance();
        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek()))) val += advance();
    }
    return Token(isF ? TokenType::T_FLOAT_LITERAL : TokenType::T_INT_LITERAL, val, line, sc);
}

Token Lexer::readString() {
    int sc = column; advance(); // opening "
    std::string val;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance(); // skip backslash
            if (isAtEnd()) break;
            char escape = advance();
            switch (escape) {
                case '"': val += '"'; break;
                case 'n': val += '\n'; break;
                case 't': val += '\t'; break;
                case '\\': val += '\\'; break;
                default: val += escape; break;
            }
        } else {
            val += advance();
        }
    }
    if (!isAtEnd()) advance(); // closing "
    return Token(TokenType::T_STRING_LITERAL, val, line, sc);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        unsigned char c = static_cast<unsigned char>(peek());
        int sc = column;
        if (std::isalpha(c) || c == '_') tokens.push_back(readIdentifier());
        else if (std::isdigit(c)) tokens.push_back(readNumber());
        else if (c == '"') tokens.push_back(readString());
        else {
            advance();
            switch (c) {
                case '(': tokens.push_back(Token(TokenType::T_LPAREN, "(", line, sc)); break;
                case ')': tokens.push_back(Token(TokenType::T_RPAREN, ")", line, sc)); break;
                case '{': tokens.push_back(Token(TokenType::T_LBRACE, "{", line, sc)); break;
                case '}': tokens.push_back(Token(TokenType::T_RBRACE, "}", line, sc)); break;
                case ',': tokens.push_back(Token(TokenType::T_COMMA, ",", line, sc)); break;
                case ';': tokens.push_back(Token(TokenType::T_SEMICOLON, ";", line, sc)); break;
                case '.': tokens.push_back(Token(TokenType::T_DOT, ".", line, sc)); break;
                case '=': 
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_EQUAL_EQUAL, "==", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_EQUALS, "=", line, sc)); 
                    break;
                case '!':
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_BANG_EQUAL, "!=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_BANG, "!", line, sc));
                    break;
                case '>':
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_GREATER_EQUAL, ">=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_GREATER, ">", line, sc));
                    break;
                case '<':
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_LESS_EQUAL, "<=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_LESS, "<", line, sc));
                    break;
                case '&':
                    if (peek() == '&') { advance(); tokens.push_back(Token(TokenType::T_AND_AND, "&&", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_INVALID, "&", line, sc));
                    break;
                case '|':
                    if (peek() == '|') { advance(); tokens.push_back(Token(TokenType::T_OR_OR, "||", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_INVALID, "|", line, sc));
                    break;
                case '+': 
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_PLUS_EQUAL, "+=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_PLUS, "+", line, sc)); 
                    break;
                case '-': 
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_MINUS_EQUAL, "-=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_MINUS, "-", line, sc)); 
                    break;
                case '*':
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_STAR_EQUAL, "*=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_STAR, "*", line, sc));
                    break;
                case '/':
                    if (peek() == '=') { advance(); tokens.push_back(Token(TokenType::T_SLASH_EQUAL, "/=", line, sc)); }
                    else tokens.push_back(Token(TokenType::T_SLASH, "/", line, sc));
                    break;
                case '%': tokens.push_back(Token(TokenType::T_PERCENT, "%", line, sc)); break;
                default: tokens.push_back(Token(TokenType::T_INVALID, std::string(1, c), line, sc)); break;
            }
        }
    }
    tokens.push_back(Token(TokenType::T_EOF, "", line, column));
    return tokens;
}

} // namespace Flux
