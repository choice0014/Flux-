#ifndef FLUX_LEXER_H
#define FLUX_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <map>

namespace Flux {

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    int column;

    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;

    void skipWhitespaceAndComments();
    Token readIdentifier();
    Token readNumber();
    Token readString();

    static const std::map<std::string, TokenType> keywords;
};

} // namespace Flux

#endif
