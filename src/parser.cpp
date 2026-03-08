#include "parser.hpp"
#include "token.hpp"

namespace deviant {

auto Parser::parse() -> std::unique_ptr<Program> {
    std::unique_ptr<Program> program = std::make_unique<Program>();
    while (index_ < lexer_.get_tokens().size()) {
        auto statement_ptr = parseTopLevelStatement();
        if (statement_ptr) { program->pushBack(std::move(statement_ptr)); }
        consume();
    }
    return program;
}

auto Parser::parseExpression() -> std::unique_ptr<Expression> {
    if (peek().has_value() && peek().value().value.has_value()) {
        Token              token(peek().value());
        const std::string& value(token.value.value());
        switch (peek().value().type) {
        case TokenType::INT_LIT:
            return std::make_unique<Integer>(stoi(value));
        case TokenType::IDENTIFIER:
            if (peek(1).value().type == TokenType::OPEN_PAREN) {
                // TODO: remove dangerous code
                consume();
                return parseFunctionCall();
            } else {
                return parseIdentifier();
            }
        default:
            return nullptr;
        }
    } else
        return nullptr;
}

auto Parser::parseTopLevelStatement() -> std::unique_ptr<Statement> {
    TokenType type =
        peek().has_value() ? peek().value().type : TokenType::ILLEGAL;

    switch (type) {
    case TokenType::FN:
        return parseFunctionStatement();
    default:
        return nullptr;
    }
}

auto Parser::parseStatement() -> std::unique_ptr<Statement> {
    TokenType type =
        peek().has_value() ? peek().value().type : TokenType::ILLEGAL;

    switch (type) {
    case TokenType::VAR: // declaration of variable
        if (peek(1).has_value() &&
            peek(1).value().type == TokenType::IDENTIFIER) {
            consume();
            return parseVariableDeclaration(); // deal with assignment later
        } else {
            return nullptr;
        }
    case TokenType::IDENTIFIER:
        if (peek(1).has_value() &&
            peek(1).value().type == TokenType::OPEN_PAREN) {
            consume();
            auto fn_call = parseFunctionCall();
            consume();
            return fn_call;
        } else { // TODO:
            consume();
            return parseAssignment();
        }
    case TokenType::ASSIGNMENT:
        return parseAssignment();
    case TokenType::IF:
        return parseIfStatement();
    case TokenType::RETURN:
        return parseReturnStatement();
    default:
        return nullptr;
    }
}

auto Parser::parseIdentifier() -> std::unique_ptr<Identifier> {
    auto identifier =
        std::make_unique<Identifier>(peek().value().value.value());

    return identifier;
}

auto Parser::parseVariableDeclaration()
    -> std::unique_ptr<VariableDeclaration> {
    if (peek().has_value() && peek().value().value.has_value()) {
        auto identifier =
            std::make_unique<Identifier>(peek().value().value.value());

        std::unique_ptr<Expression> expr(nullptr);
        consume();
        auto token_type = peek().value().type;
        switch (token_type) {
        case TokenType::SEMICOLON:
            break;
        case TokenType::ASSIGNMENT: // TODO:
            --index_;
            break;
        default:
            return nullptr;
        }
        auto var_decl = std::make_unique<VariableDeclaration>(
            std::move(identifier), std::move(expr));
        return var_decl;
    } else {
        return nullptr;
    }
}

auto Parser::parseAssignment() -> std::unique_ptr<Assignment> {
    auto assign = std::make_unique<Assignment>();

    // TODO: peek(-1) is dangerous
    assign->setVarname(peek(-1).value().value.value());
    consume();

    auto expr = parseExpression();
    assign->setExpression(std::move(expr));

    consume();

    return assign;
}

auto Parser::parseBlock() -> std::unique_ptr<Block> {
    auto block = std::make_unique<Block>();

    auto stmt = parseStatement();
    while (stmt) {
        block->insertStatement(std::move(stmt));
        consume();
        stmt = parseStatement();
    }

    return block;
}

auto Parser::parseReturnStatement() -> std::unique_ptr<ReturnStatement> {
    consume();
    auto ret_stmt = std::make_unique<ReturnStatement>(parseExpression());

    if (consume().type == TokenType::SEMICOLON) return nullptr;

    return ret_stmt;
}

auto Parser::parseInfixStatement() -> std::unique_ptr<ComparationOp> {
    return nullptr;
    // return std::unique_ptr<ComparationOp>();
}

auto Parser::parseIfStatement() -> std::unique_ptr<IfStatement> {
    std::unique_ptr<IfStatement> if_stmt = std::make_unique<IfStatement>();

    // condition
    consume(); // TokenType::IF
    consume(); // TokenType::OPEN_PAREN
    if_stmt->setCondition(parseExpression());
    consume(); // condition
    consume(); // TokenType::CLOSE_PAREN

    // then
    consume(); // TokenType::OPEN_CURLY
    if_stmt->setThenBlock(parseBlock());

    // else
    if (peek().has_value() && peek().value().type == TokenType::ELSE) {
        consume(); // TokenType::CLOSE_CURLY
        consume(); // TokenType::ELSE
        consume(); // TokenType::OPEN_CURLY
        if_stmt->setElseBlock(parseBlock());
        // consume();  // TokenType::CLOSE_CURLY
    }

    return if_stmt;
}

auto Parser::parseFunctionStatement() -> std::unique_ptr<FunctionStatement> {
    consume();
    if (peek().value().value.has_value()) {
        auto fn =
            std::make_unique<FunctionStatement>(peek().value().value.value());

        // TODO: parameters
        consume();
        consume();
        consume();

        if (peek().value().type != TokenType::FN_TYPE) return nullptr;

        // TODO: return type
        consume();
        consume();

        if (consume().type == TokenType::OPEN_CURLY) {
            fn->setBlock(parseBlock());
        }
        return fn;
    } else {
        return nullptr;
    }
}

auto Parser::parseFunctionCall() -> std::unique_ptr<FunctionCall> {
    auto fn_call =
        std::make_unique<FunctionCall>(peek(-1).value().value.value());

    consume();
    // prase arguments
    while (peek().has_value() &&
           peek().value().type != TokenType::CLOSE_PAREN) {
        fn_call->addArgument(parseExpression());
        consume();
        if (peek().has_value() && peek().value().type == TokenType::COMMA) {
            consume(); // TODO: ,) should be forbidden
        }
    }
    // consume(); // skip close paren
    // consume(); // skip semicolon

    return fn_call;
}

auto Parser::peek(const int offset) const -> std::optional<Token> {
    const auto& tokens = lexer_.get_tokens();
    if (index_ + offset >= tokens.size())
        return std::nullopt;
    else
        return tokens[index_ + offset];
}

auto Parser::consume() -> const Token& { return lexer_.get_tokens()[index_++]; }
} // namespace deviant