#ifndef FLUX_AST_H
#define FLUX_AST_H

#include <string>
#include <vector>
#include <memory>
#include "Token.h"

namespace Flux {
namespace AST {

class Node { public: virtual ~Node() = default; };
class Expression : public Node {};
class Statement : public Node {};

class IntLiteral : public Expression { public: int value; IntLiteral(int v) : value(v) {} };
class FloatLiteral : public Expression { public: float value; FloatLiteral(float v) : value(v) {} };
class StringLiteral : public Expression { public: std::string value; StringLiteral(std::string v) : value(v) {} };
class BooleanLiteral : public Expression { public: bool value; BooleanLiteral(bool v) : value(v) {} };
class VariableExpr : public Expression { public: std::string name; VariableExpr(std::string n) : name(n) {} };

class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
    BinaryExpr(std::unique_ptr<Expression> l, Token o, std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

class Assignment : public Expression {
public:
    std::string name;
    std::unique_ptr<Expression> value;
    Assignment(std::string n, std::unique_ptr<Expression> v)
        : name(n), value(std::move(v)) {}
};

class UnaryExpr : public Expression {
public:
    Token op;
    std::unique_ptr<Expression> operand;
    UnaryExpr(Token o, std::unique_ptr<Expression> expr) : op(o), operand(std::move(expr)) {}
};

class CallExpr : public Expression {
public:
    std::string callee;
    std::vector<std::unique_ptr<Expression>> args;
    CallExpr(std::string c, std::vector<std::unique_ptr<Expression>> a)
        : callee(c), args(std::move(a)) {}
};

struct VariableDeclaration {
    std::string name;
    std::unique_ptr<Expression> initializer;
};

class VarDeclaration : public Statement {
public:
    std::string type;
    std::vector<std::unique_ptr<VariableDeclaration>> decls;
    VarDeclaration(std::string t, std::vector<std::unique_ptr<VariableDeclaration>> d)
        : type(t), decls(std::move(d)) {}
};

class BlockStmt : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStmt(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts)) {}
};

class IfStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
    IfStmt(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> thenB, std::unique_ptr<Statement> elseB)
        : condition(std::move(cond)), thenBranch(std::move(thenB)), elseBranch(std::move(elseB)) {}
};

class WhileStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    WhileStmt(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

class ReturnStmt : public Statement {
public:
    std::unique_ptr<Expression> value;
    ReturnStmt(std::unique_ptr<Expression> v) : value(std::move(v)) {}
};

class ExpressionStmt : public Statement {
public:
    std::unique_ptr<Expression> expr;
    ExpressionStmt(std::unique_ptr<Expression> e) : expr(std::move(e)) {}
};

class FunctionDef : public Statement {
public:
    std::string name;
    struct Param { std::string type; std::string name; };
    std::vector<Param> params;
    std::vector<std::unique_ptr<Statement>> body;
    FunctionDef(std::string n, std::vector<Param> p, std::vector<std::unique_ptr<Statement>> b)
        : name(n), params(p), body(std::move(b)) {}
};

class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace AST
} // namespace Flux

#endif
