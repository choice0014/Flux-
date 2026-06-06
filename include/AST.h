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

class PostfixExpr : public Expression {
public:
    std::string name;
    Token op;
    PostfixExpr(std::string n, Token o) : name(n), op(o) {}
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

class PrintfExpr : public Expression {
public:
    struct Part { bool isExpression; std::string text; std::unique_ptr<Expression> expr; };
    std::vector<Part> parts;
    PrintfExpr(std::vector<Part> p) : parts(std::move(p)) {}
};

class MemberAccessExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string memberName;
    MemberAccessExpr(std::unique_ptr<Expression> obj, std::string member)
        : object(std::move(obj)), memberName(member) {}
};

class MemberAssignment : public Expression {
public:
    std::unique_ptr<MemberAccessExpr> memberAccess;
    std::unique_ptr<Expression> value;
    MemberAssignment(std::unique_ptr<MemberAccessExpr> access, std::unique_ptr<Expression> val)
        : memberAccess(std::move(access)), value(std::move(val)) {}
};

class NewExpr : public Expression {
public:
    std::string className;
    std::vector<std::unique_ptr<Expression>> args;
    NewExpr(std::string name, std::vector<std::unique_ptr<Expression>> a)
        : className(name), args(std::move(a)) {}
};

class ThisExpr : public Expression {};

class ArrayLiteralExpr : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> elements;
    ArrayLiteralExpr(std::vector<std::unique_ptr<Expression>> el) : elements(std::move(el)) {}
};

class ArrayIndexExpr : public Expression {
public:
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;
    ArrayIndexExpr(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx)
        : array(std::move(arr)), index(std::move(idx)) {}
};

class ArrayIndexAssignment : public Expression {
public:
    std::unique_ptr<ArrayIndexExpr> arrayIndex;
    std::unique_ptr<Expression> value;
    ArrayIndexAssignment(std::unique_ptr<ArrayIndexExpr> ai, std::unique_ptr<Expression> v)
        : arrayIndex(std::move(ai)), value(std::move(v)) {}
};

class VarDeclaration : public Statement {
public:
    std::string type;
    std::vector<std::string> names;
    bool isArray;
    VarDeclaration(std::string t, std::vector<std::string> n, bool isArr = false) 
        : type(t), names(n), isArray(isArr) {}
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

class ForStmt : public Statement {
public:
    std::unique_ptr<Statement> init;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> update;
    std::unique_ptr<Statement> body;
    ForStmt(std::unique_ptr<Statement> i, std::unique_ptr<Expression> c, std::unique_ptr<Expression> u, std::unique_ptr<Statement> b)
        : init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {}
};

class ReturnStmt : public Statement {
public:
    std::unique_ptr<Expression> value;
    ReturnStmt(std::unique_ptr<Expression> v) : value(std::move(v)) {}
};

class BreakStmt : public Statement {
public:
    BreakStmt() = default;
};

class SwitchStmt : public Statement {
public:
    std::unique_ptr<Expression> discriminant;
    struct Case {
        std::unique_ptr<Expression> value; // nullptr for default
        std::vector<std::unique_ptr<Statement>> body;
    };
    std::vector<Case> cases;
    SwitchStmt(std::unique_ptr<Expression> disc, std::vector<Case> cs)
        : discriminant(std::move(disc)), cases(std::move(cs)) {}
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

class ImportStmt : public Statement {
public:
    std::string moduleName;
    ImportStmt(std::string m) : moduleName(m) {}
};

class ImportFromStmt : public Statement {
public:
    std::string functionName;
    std::string fileName;
    ImportFromStmt(std::string func, std::string file) : functionName(func), fileName(file) {}
};

class StructDef : public Statement {
public:
    std::string name;
    struct Member { std::string type; std::string name; };
    std::vector<Member> members;
    StructDef(std::string n, std::vector<Member> m) : name(n), members(std::move(m)) {}
};

class ClassDef : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<VarDeclaration>> properties;
    std::vector<std::unique_ptr<FunctionDef>> methods;
    ClassDef(std::string n, std::vector<std::unique_ptr<VarDeclaration>> p, std::vector<std::unique_ptr<FunctionDef>> m)
        : name(n), properties(std::move(p)), methods(std::move(m)) {}
};

class ThrowStmt : public Statement {
public:
    std::unique_ptr<Expression> value;
    ThrowStmt(std::unique_ptr<Expression> v) : value(std::move(v)) {}
};

class TryCatchStmt : public Statement {
public:
    std::unique_ptr<Statement> tryBlock;
    std::string exceptionVarName;
    std::unique_ptr<Statement> catchBlock;
    TryCatchStmt(std::unique_ptr<Statement> t, std::string var, std::unique_ptr<Statement> c)
        : tryBlock(std::move(t)), exceptionVarName(var), catchBlock(std::move(c)) {}
};

class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace AST
} // namespace Flux

#endif
