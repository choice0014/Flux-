#ifndef FLUX_PARSER_H
#define FLUX_PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace Flux {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<AST::Program> parse();

private:
    std::vector<Token> tokens;
    size_t pos;

    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& message);

    std::unique_ptr<AST::Statement> parseStatement();
    std::vector<std::unique_ptr<AST::Statement>> parseBlock();
    std::unique_ptr<AST::Statement> parseIfStmt();
    std::unique_ptr<AST::Statement> parseWhileStmt();
    std::unique_ptr<AST::Statement> parseReturnStmt();
    std::unique_ptr<AST::Statement> parseFunctionDef();
    std::unique_ptr<AST::Statement> parseVarDeclaration();

    std::unique_ptr<AST::Expression> parseExpression();
    std::unique_ptr<AST::Expression> parseAssignment();
    std::unique_ptr<AST::Expression> parseLogicalOr();
    std::unique_ptr<AST::Expression> parseLogicalAnd();
    std::unique_ptr<AST::Expression> parseComparison();
    std::unique_ptr<AST::Expression> parseAdditive();
    std::unique_ptr<AST::Expression> parseMultiplicative();
    std::unique_ptr<AST::Expression> parseUnary();
    std::unique_ptr<AST::Expression> parsePrimary();
    std::unique_ptr<AST::Expression> parsePostfix();
};

} // namespace Flux

#endif
