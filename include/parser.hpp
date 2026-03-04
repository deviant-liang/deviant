#ifndef __PARSER_H__
#define __PARSER_H__

#include <map>

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
    std::unique_ptr<Program> parse();

private:
    // TODO: lots of things...
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Statement> parseTopLevelStatement();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Identifier> parseIdentifier();
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<Assignment> parseAssignment();
    std::unique_ptr<FunctionStatement> parseFunctionStatement();
    std::unique_ptr<FunctionCall> parseFunctionCall();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ComparationOp> parseInfixStatement();
    std::unique_ptr<IfStatement> parseIfStatement();
    std::unique_ptr<Block> parseBlock();

    [[nodiscard]] std::optional<Token> peek(int offset = 0) const;

    const Token& consume();

    Lexer lexer_;

    size_t index_;
};

}  // namespace deviant

#endif
