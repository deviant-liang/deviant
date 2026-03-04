#include "parser.hpp"
#include "token.hpp"

namespace deviant {
std::unique_ptr<Program> Parser::parse() {
  std::unique_ptr<Program> program = std::make_unique<Program>();
  while (index_ < lexer_.getTokens().size()) {
    auto statement_ptr = parseTopLevelStatement();
    if (statement_ptr) {
      program->pushBack(std::move(statement_ptr));
    }
    consume();
  }
  return program;
}

std::unique_ptr<Expression> Parser::parseExpression() {
  if (peek().has_value() && peek().value().value.has_value()) {
    Token token(peek().value());
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

std::unique_ptr<Statement> Parser::parseTopLevelStatement() {
  TokenType type =
      peek().has_value() ? peek().value().type : TokenType::ILLEGAL;

  switch (type) {
    case TokenType::FN:
      return parseFunctionStatement();
    default:
      return nullptr;
  }
}

std::unique_ptr<Statement> Parser::parseStatement() {
  TokenType type =
      peek().has_value() ? peek().value().type : TokenType::ILLEGAL;

  switch (type) {
    case TokenType::VAR:  // declaration of variable
      if (peek(1).has_value() &&
          peek(1).value().type == TokenType::IDENTIFIER) {
        consume();
        return parseVariableDeclaration();  // deal with assignment later
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
      } else {  // TODO:
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

std::unique_ptr<Identifier> Parser::parseIdentifier() {
  auto identifier = std::make_unique<Identifier>(peek().value().value.value());

  return identifier;
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
  if (peek().has_value() && peek().value().value.has_value()) {
    auto identifier =
        std::make_unique<Identifier>(peek().value().value.value());

    std::unique_ptr<Expression> expr(nullptr);
    consume();
    auto token_type = peek().value().type;
    switch (token_type) {
      case TokenType::SEMICOLON:
        break;
      case TokenType::ASSIGNMENT:  // TODO:
        --index_;
        break;
      default:
        return nullptr;
    }
    auto var_decl = std::make_unique<VariableDeclaration>(std::move(identifier),
                                                          std::move(expr));
    return var_decl;
  } else {
    return nullptr;
  }
}

std::unique_ptr<Assignment> Parser::parseAssignment() {
  auto assign = std::make_unique<Assignment>();

  // TODO: peek(-1) is dangerous
  assign->setVarname(peek(-1).value().value.value());
  consume();

  auto expr = parseExpression();
  assign->setExpression(std::move(expr));

  consume();

  return assign;
}

std::unique_ptr<Block> Parser::parseBlock() {
  auto block = std::make_unique<Block>();

  auto stmt = parseStatement();
  while (stmt) {
    block->insertStatement(std::move(stmt));
    consume();
    stmt = parseStatement();
  }

  return block;
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
  consume();
  auto ret_stmt = std::make_unique<ReturnStatement>(parseExpression());

  if (consume().type == TokenType::SEMICOLON)
    return nullptr;

  return ret_stmt;
}

std::unique_ptr<ComparationOp> Parser::parseInfixStatement() {
  return nullptr;
  // return std::unique_ptr<ComparationOp>();
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
  std::unique_ptr<IfStatement> if_stmt = std::make_unique<IfStatement>();

  // condition
  consume();  // TokenType::IF
  consume();  // TokenType::OPEN_PAREN
  if_stmt->setCondition(parseExpression());
  consume();  // condition
  consume();  // TokenType::CLOSE_PAREN

  // then
  consume();  // TokenType::OPEN_CURLY
  if_stmt->setThenBlock(parseBlock());

  // else
  if (peek().has_value() && peek().value().type == TokenType::ELSE) {
    consume();  // TokenType::CLOSE_CURLY
    consume();  // TokenType::ELSE
    consume();  // TokenType::OPEN_CURLY
    if_stmt->setElseBlock(parseBlock());
    // consume();  // TokenType::CLOSE_CURLY
  }

  return if_stmt;
}

std::unique_ptr<FunctionStatement> Parser::parseFunctionStatement() {
  consume();
  if (peek().value().value.has_value()) {
    auto fn = std::make_unique<FunctionStatement>(peek().value().value.value());

    // TODO: parameters
    consume();
    consume();
    consume();

    if (peek().value().type != TokenType::FN_TYPE)
      return nullptr;

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

std::unique_ptr<FunctionCall> Parser::parseFunctionCall() {
  auto fn_call = std::make_unique<FunctionCall>(peek(-1).value().value.value());

  consume();
  // prase arguments
  while (peek().has_value() && peek().value().type != TokenType::CLOSE_PAREN) {
    fn_call->addArgument(parseExpression());
    consume();
    if (peek().has_value() && peek().value().type == TokenType::COMMA) {
      consume();  // TODO: ,) should be forbidden
    }
  }
  // consume(); // skip close paren
  // consume(); // skip semicolon

  return fn_call;
}

std::optional<Token> Parser::peek(const int offset) const {
  const auto& tokens = lexer_.getTokens();
  if (index_ + offset >= tokens.size())
    return std::nullopt;
  else
    return tokens[index_ + offset];
}

const Token& Parser::consume() {
  return lexer_.getTokens()[index_++];
}
}  // namespace deviant