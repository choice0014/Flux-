#include "Parser.h"
#include "Lexer.h"
#include <stdexcept>

namespace Flux {

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

const Token& Parser::peek() const { return tokens[pos]; }
const Token& Parser::previous() const { return tokens[pos - 1]; }
const Token& Parser::advance() { if (!isAtEnd()) pos++; return previous(); }

bool Parser::isAtEnd() const { return peek().type == TokenType::T_EOF; }
bool Parser::check(TokenType type) const { return !isAtEnd() && peek().type == type; }
bool Parser::match(TokenType type) { if (check(type)) { advance(); return true; } return false; }

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error("Error at [" + std::to_string(peek().line) + ":" + std::to_string(peek().column) + "]: " + message + " (Got: " + peek().value + ")");
}

std::unique_ptr<AST::Program> Parser::parse() {
    auto program = std::make_unique<AST::Program>();
    while (!isAtEnd()) program->statements.push_back(parseStatement());
    return program;
}

std::unique_ptr<AST::Statement> Parser::parseStatement() {
    if (match(TokenType::T_FUNCTION)) return parseFunctionDef();
    if (match(TokenType::T_IF)) return parseIfStmt();
    if (match(TokenType::T_WHILE)) return parseWhileStmt();
    if (match(TokenType::T_LBRACE)) return std::make_unique<AST::BlockStmt>(parseBlock());

    // 변수 선언 체크 (IDENTIFIER IDENTIFIER 패턴만 선언으로 간주)
    if (check(TokenType::T_INT) || check(TokenType::T_FLOAT) || check(TokenType::T_STRING) || check(TokenType::T_BOOL) ||
        (check(TokenType::T_IDENTIFIER) && pos + 1 < tokens.size() && tokens[pos+1].type == TokenType::T_IDENTIFIER)) {
        
        advance();
        return parseVarDeclaration();
    }
    if (match(TokenType::T_RETURN)) return parseReturnStmt();

    auto expr = parseExpression();
    consume(TokenType::T_SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<AST::ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<AST::Statement>> Parser::parseBlock() {
    std::vector<std::unique_ptr<AST::Statement>> statements;
    while (!check(TokenType::T_RBRACE) && !isAtEnd()) {
        statements.push_back(parseStatement());
    }
    consume(TokenType::T_RBRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<AST::Statement> Parser::parseIfStmt() {
    consume(TokenType::T_LPAREN, "Expect '(' after 'if'.");
    auto condition = parseExpression();
    consume(TokenType::T_RPAREN, "Expect ')' after condition.");
    auto thenBranch = parseStatement();
    std::unique_ptr<AST::Statement> elseBranch = nullptr;
    if (match(TokenType::T_ELSE)) {
        elseBranch = parseStatement();
    }
    return std::make_unique<AST::IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<AST::Statement> Parser::parseWhileStmt() {
    consume(TokenType::T_LPAREN, "Expect '(' after 'while'.");
    auto condition = parseExpression();
    consume(TokenType::T_RPAREN, "Expect ')' after condition.");
    auto body = parseStatement();
    return std::make_unique<AST::WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<AST::Statement> Parser::parseReturnStmt() {
    consume(TokenType::T_LPAREN, "Expect '(' after 'return'.");
    std::unique_ptr<AST::Expression> value = nullptr;
    if (!check(TokenType::T_RPAREN)) value = parseExpression();
    consume(TokenType::T_RPAREN, "Expect ')' after return value.");
    consume(TokenType::T_SEMICOLON, "Expect ';' after return call.");
    return std::make_unique<AST::ReturnStmt>(std::move(value));
}

std::unique_ptr<AST::Statement> Parser::parseFunctionDef() {
    std::string name = consume(TokenType::T_IDENTIFIER, "Expect function name.").value;
    consume(TokenType::T_LPAREN, "Expect '(' after function name.");
    std::vector<AST::FunctionDef::Param> params;
    if (!check(TokenType::T_RPAREN)) {
        do {
            std::string type;
            if (match(TokenType::T_INT)) type = "int";
            else if (match(TokenType::T_FLOAT)) type = "float";
            else if (match(TokenType::T_STRING)) type = "string";
            else if (match(TokenType::T_BOOL)) type = "bool";
            else throw std::runtime_error("Expect parameter type.");

            std::string pName = consume(TokenType::T_IDENTIFIER, "Expect parameter name.").value;
            params.push_back({type, pName});
        } while (match(TokenType::T_COMMA));
    }
    consume(TokenType::T_RPAREN, "Expect ')' after parameters.");
    consume(TokenType::T_LBRACE, "Expect '{' before function body.");
    return std::make_unique<AST::FunctionDef>(name, params, parseBlock());
}

std::unique_ptr<AST::Statement> Parser::parseVarDeclaration() {
    std::string type = previous().value;
    std::vector<std::unique_ptr<AST::VariableDeclaration>> decls;
    
    do {
        std::string name = consume(TokenType::T_IDENTIFIER, "Expect variable name.").value;
        std::unique_ptr<AST::Expression> initializer = nullptr;
        
        if (match(TokenType::T_EQUALS)) {
            initializer = parseExpression();
        }
        
        auto decl = std::make_unique<AST::VariableDeclaration>();
        decl->name = name;
        decl->initializer = std::move(initializer);
        decls.push_back(std::move(decl));
    } while (match(TokenType::T_COMMA));

    consume(TokenType::T_SEMICOLON, "Expect ';' after variable declaration.");
    
    return std::make_unique<AST::VarDeclaration>(type, std::move(decls));
}

std::unique_ptr<AST::Expression> Parser::parseExpression() { return parseAssignment(); }

std::unique_ptr<AST::Expression> Parser::parseAssignment() {
    auto expr = parseLogicalOr();
    if (match(TokenType::T_EQUALS)) {
        auto value = parseAssignment();
        if (AST::VariableExpr* var = dynamic_cast<AST::VariableExpr*>(expr.get())) {
            return std::make_unique<AST::Assignment>(var->name, std::move(value));
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::T_OR_OR)) {
        Token op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_unique<AST::BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseLogicalAnd() {
    auto expr = parseComparison();
    while (match(TokenType::T_AND_AND)) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<AST::BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseComparison() {
    auto expr = parseAdditive();
    while (match(TokenType::T_EQUAL_EQUAL) || match(TokenType::T_BANG_EQUAL) ||
           match(TokenType::T_GREATER) || match(TokenType::T_GREATER_EQUAL) ||
           match(TokenType::T_LESS) || match(TokenType::T_LESS_EQUAL)) {
        Token op = previous();
        auto right = parseAdditive();
        expr = std::make_unique<AST::BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    while (match(TokenType::T_PLUS) || match(TokenType::T_MINUS)) {
        Token op = previous();
        auto right = parseMultiplicative();
        expr = std::make_unique<AST::BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseMultiplicative() {
    auto expr = parseUnary();
    while (match(TokenType::T_STAR) || match(TokenType::T_SLASH)) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_unique<AST::BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseUnary() {
    if (match(TokenType::T_MINUS) || match(TokenType::T_BANG)) {
        Token op = previous();
        auto operand = parseUnary();
        return std::make_unique<AST::UnaryExpr>(op, std::move(operand));
    }
    return parsePostfix();
}

std::unique_ptr<AST::Expression> Parser::parsePostfix() {
    auto expr = parsePrimary();
    while (true) {
        if (match(TokenType::T_LPAREN)) {
            std::vector<std::unique_ptr<AST::Expression>> args;
            if (!check(TokenType::T_RPAREN)) {
                do { args.push_back(parseExpression()); } while (match(TokenType::T_COMMA));
            }
            consume(TokenType::T_RPAREN, "Expect ')' after arguments.");

            if (AST::VariableExpr* v = dynamic_cast<AST::VariableExpr*>(expr.get())) {
                if (v->name == "printf") {
                    if (args.empty()) throw std::runtime_error("printf expects a format string.");
                }
                expr = std::make_unique<AST::CallExpr>(v->name, std::move(args));
            } else {
                throw std::runtime_error("Invalid call target.");
            }
        } else {
            break;
        }
    }
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parsePrimary() {
    if (match(TokenType::T_INT_LITERAL)) return std::make_unique<AST::IntLiteral>(std::stoi(previous().value));
    if (match(TokenType::T_FLOAT_LITERAL)) return std::make_unique<AST::FloatLiteral>(std::stof(previous().value));
    if (match(TokenType::T_STRING_LITERAL)) return std::make_unique<AST::StringLiteral>(previous().value);
    if (match(TokenType::T_TRUE)) return std::make_unique<AST::BooleanLiteral>(true);
    if (match(TokenType::T_FALSE)) return std::make_unique<AST::BooleanLiteral>(false);
    if (match(TokenType::T_IDENTIFIER)) {
        std::string name = previous().value;
        return std::make_unique<AST::VariableExpr>(name);
    }
    if (match(TokenType::T_LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::T_RPAREN, ")"); return expr;
    }
    throw std::runtime_error("Expect expression.");
}

} // namespace Flux
