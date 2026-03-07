#ifndef __PARSER_H__
#define __PARSER_H__

#include "ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

namespace deviant {
class Parser {
public:
    Parser(const std::string& content) : lexer_(content), index_(0) {
        lexer_.tokenize();
    }

    // parse whole program
    auto parse() -> std::unique_ptr<Program>;

private:
    // TODO: lots of things...
    auto parseExpression() -> std::unique_ptr<Expression>;
    auto parseTopLevelStatement() -> std::unique_ptr<Statement>;
    auto parseStatement() -> std::unique_ptr<Statement>;
    auto parseIdentifier() -> std::unique_ptr<Identifier>;
    auto parseVariableDeclaration() -> std::unique_ptr<VariableDeclaration>;
    auto parseAssignment() -> std::unique_ptr<Assignment>;
    auto parseFunctionStatement() -> std::unique_ptr<FunctionStatement>;
    auto parseFunctionCall() -> std::unique_ptr<FunctionCall>;
    auto parseReturnStatement() -> std::unique_ptr<ReturnStatement>;
    auto parseInfixStatement() -> std::unique_ptr<ComparationOp>;
    auto parseIfStatement() -> std::unique_ptr<IfStatement>;
    auto parseBlock() -> std::unique_ptr<Block>;

    [[nodiscard]] auto peek(int offset = 0) const -> std::optional<Token>;

    auto consume() -> const Token&;

    Lexer  lexer_;
    size_t index_;
};

} // namespace deviant

#endif
