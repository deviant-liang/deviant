#ifndef __AST__
#define __AST__

#include <memory>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(push, 0)
#endif

#include "llvm/IR/Value.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace deviant {

class DeviantLLVM;

class AstNode {
public:
    enum class Type {
        PROGRAM,
        EXPRESSTION,
        STATEMENT,
        INTEGER,
        DECIMAL,
        BOOLEAN,
        IDENTIFIER
    };

    AstNode() noexcept = default;
    virtual ~AstNode() noexcept = default;

    // TODO: pure virtual
    virtual auto generateCode(DeviantLLVM& context) -> llvm::Value*;
    virtual auto type() -> Type = 0;
    virtual auto toString() -> std::string = 0;
};

class Expression : public AstNode {
public:
    ~Expression() override = default;

    auto generateCode(DeviantLLVM& context) -> llvm::Value* override = 0;
    auto type() -> Type override {
        return Type::EXPRESSTION;
    }
    std::string toString() override {
        return "Expression";
    }
};

class Statement : public Expression {
public:
    ~Statement() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override = 0;
    Type         type() override {
        return Type::EXPRESSTION;
    }
    std::string toString() override {
        return "Statement";
    }
};

class Program : public AstNode {
public:
    ~Program() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    Type         type() override {
        return Type::PROGRAM;
    }
    std::string toString() override {
        return "Program";
    }

    void pushBack(std::unique_ptr<Statement>&& statement) {
        statements_.emplace_back(std::move(statement));
    }

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

class Integer : public Expression {
public:
    explicit Integer(int value) : value_(value) {
    }
    ~Integer() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    Type         type() override {
        return Type::INTEGER;
    }
    std::string toString() override {
        return " ";
    }

private:
    int value_;
};

class Identifier : public Expression {
public:
    explicit Identifier(const std::string& name) : name_(name) {
    }
    ~Identifier() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    Type         type() override {
        return Type::IDENTIFIER;
    }
    std::string toString() override {
        return "identifier";
    }

    const std::string& getName() {
        return name_;
    }

private:
    std::string name_;
};

class VariableDeclaration : public Statement {
public:
    VariableDeclaration(std::unique_ptr<Identifier>&& identifier,
                        std::unique_ptr<Expression>&& expr)
        : identifier_(std::move(identifier)), expr_(std::move(expr)) {
    }
    ~VariableDeclaration() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    Type         type() override {
        return Type::STATEMENT;
    }
    std::string toString() override {
        return "let";
    }
    void setIdentifier(std::unique_ptr<Identifier>&& identifier) {
        identifier_ = std::move(identifier);
    }
    void setExpression(std::unique_ptr<Expression>&& expr) {
        expr_ = std::move(expr);
    }

private:
    std::unique_ptr<Identifier> identifier_;
    std::unique_ptr<Expression> expr_;
};

class Assignment : public Statement {
public:
    explicit Assignment() {
    }
    ~Assignment() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    Type         type() override {
        return Type::STATEMENT;
    }
    std::string toString() override {
        return "var";
    }
    void setVarname(const std::string& name) {
        var_name_ = name;
    }
    void setExpression(std::unique_ptr<Expression>&& expr) {
        expr_ = std::move(expr);
    }

private:
    // std::unique_ptr<Identifier> identifier_;
    std::string                 var_name_;
    std::unique_ptr<Expression> expr_;
};

class Block : public Expression {
public:
    ~Block() override = default;

    Type type() override {
        return Type::EXPRESSTION;
    }
    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "block";
    }
    void insertStatement(std::unique_ptr<Statement>&& stmt) {
        statements_.push_back(std::move(stmt));
    }

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

class ReturnStatement : public Statement {
public:
    explicit ReturnStatement(std::unique_ptr<Expression>&& expr)
        : ret_expr_(std::move(expr)) {
    }
    ~ReturnStatement() override = default;
    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "return";
    }

private:
    std::unique_ptr<Expression> ret_expr_;
};

class FunctionStatement : public Statement {
public:
    explicit FunctionStatement(const std::string& fn_name) : fn_name_(fn_name) {
    }
    ~FunctionStatement() override = default;
    Type type() override {
        return Type::STATEMENT;
    }
    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "fn";
    }
    void setBlock(std::unique_ptr<Block>&& body) {
        body_ = std::move(body);
    }

private:
    std::string            fn_name_;
    std::unique_ptr<Block> body_;
};

class FunctionCall : public Statement {
public:
    explicit FunctionCall(const std::string& fn_name) : fn_name_(fn_name) {
    }
    ~FunctionCall() override = default;
    Type type() override {
        return Type::STATEMENT;
    }
    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "fn call";
    }

    void addArgument(std::unique_ptr<Expression>&& arg) {
        args_.emplace_back(std::move(arg));
    }

private:
    std::string                              fn_name_;
    std::vector<std::unique_ptr<Expression>> args_;
};

class ComparationOp : public Expression {
public:
    enum CompOp { LT, LE, GT, GE, EQ, NE };

    explicit ComparationOp(std::unique_ptr<Expression>&& lhs, CompOp op,
                           std::unique_ptr<Expression>&& rhs)
        : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
    }

    ~ComparationOp() override = default;

    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "";
    }

    CompOp getOperator() const {
        return op_;
    }
    Expression* getLHS() {
        return lhs_.get();
    }
    Expression* getRHS() {
        return rhs_.get();
    }

private:
    CompOp                      op_;
    std::unique_ptr<Expression> lhs_;
    std::unique_ptr<Expression> rhs_;
};

class IfStatement : public Statement {
public:
    explicit IfStatement() {
    }
    ~IfStatement() override = default;
    void setCondition(std::unique_ptr<Expression>&& condition) {
        condition_ = std::move(condition);
    }
    void setThenBlock(std::unique_ptr<Block>&& then_block) {
        then_ = std::move(then_block);
    }
    void setElseBlock(std::unique_ptr<Block>&& else_block) {
        else_ = std::move(else_block);
    }

    llvm::Value* generateCode(DeviantLLVM& context) override;
    std::string  toString() override {
        return "";
    }

private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Block>      then_;
    std::unique_ptr<Block>      else_;
};

} // namespace deviant

#endif // __AST__