#ifndef __LEXER_H__
#define __LEXER_H__

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "token.hpp"

namespace deviant {
class Lexer {
public:
    explicit Lexer(const std::string& src);

    inline auto tokenize() -> void {
        std::string buf("");
        while (peek().has_value()) {
            if (std::isalpha(peek().value())) {
                buf += consume();
                while (peek().has_value() && std::isalnum(peek().value())) {
                    buf.push_back(consume());
                }
                if (buf == "ret") {
                    tokens_.push_back({.type = TokenType::RETURN});
                    buf.clear();
                } else if (buf == "var") {
                    tokens_.push_back({.type = TokenType::VAR});
                    buf.clear();
                } else if (buf == "if") {
                    tokens_.push_back({.type = TokenType::IF});
                    buf.clear();
                } else if (buf == "else") {
                    tokens_.push_back({.type = TokenType::ELSE});
                    buf.clear();
                } else if (buf == "fn") {
                    tokens_.push_back({.type = TokenType::FN});
                    buf.clear();
                } else if (buf == "int") {
                    tokens_.push_back({.type = TokenType::INT});
                    buf.clear();
                } else {
                    tokens_.push_back(
                        {.type = TokenType::IDENTIFIER, .value = buf});
                    buf.clear();
                }
            } else if (std::isdigit(peek().value())) {
                buf.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buf.push_back(consume());
                }
                tokens_.push_back({.type = TokenType::INT_LIT, .value = buf});
                buf.clear();
            } else if (peek().value() == '(') {
                consume();
                tokens_.push_back({.type = TokenType::OPEN_PAREN});
            } else if (peek().value() == ')') {
                consume();
                tokens_.push_back({.type = TokenType::CLOSE_PAREN});
            } else if (peek().value() == ',') {
                consume();
                tokens_.push_back({.type = TokenType::COMMA});
            } else if (peek().value() == ';') {
                consume();
                tokens_.push_back({.type = TokenType::SEMICOLON});
            } else if (peek().value() == '=') {
                if (peek(1).has_value() &&
                    peek(1).value() == '=') { // deal with comment
                    consume();
                    consume();
                    tokens_.push_back({.type = TokenType::EQ});
                } else {
                    consume();
                    tokens_.push_back({.type = TokenType::ASSIGNMENT});
                }
            } else if (peek().value() == '+') {
                consume();
                tokens_.push_back({.type = TokenType::PLUS});
            } else if (peek().value() == '*') {
                consume();
                tokens_.push_back({.type = TokenType::STAR});
            } else if (peek().value() == '-') {
                consume();
                if (peek().value() == '>') {
                    consume();
                    tokens_.push_back({.type = TokenType::FN_TYPE});
                } else {
                    tokens_.push_back({.type = TokenType::MINUS});
                }
            } else if (peek().value() == '/') {
                if (peek(-1).value() == '/') { // deal with comment
                    tokens_.pop_back();
                    char c = '/';
                    while (c != '\n') {
                        c = consume();
                    }
                } else {
                    consume();
                    tokens_.push_back({.type = TokenType::FSLASH});
                }
            } else if (peek().value() == '<') {
                if (peek(1).has_value() &&
                    peek(1).value() == '=') { // deal with comment
                    consume();
                    consume();
                    tokens_.push_back({.type = TokenType::LE});
                } else {
                    consume();
                    tokens_.push_back({.type = TokenType::LT});
                }
            } else if (peek().value() == '>') {
                if (peek(1).has_value() &&
                    peek(1).value() == '=') { // deal with comment
                    consume();
                    consume();
                    tokens_.push_back({.type = TokenType::GE});
                } else {
                    consume();
                    tokens_.push_back({.type = TokenType::GT});
                }
            } else if (peek().value() == '!') {
                if (peek(1).has_value() &&
                    peek(1).value() == '=') { // deal with comment
                    consume();
                    consume();
                    tokens_.push_back({.type = TokenType::NE});
                } else {
                    consume();
                    tokens_.push_back({.type = TokenType::EXCLAMATION});
                }
            } else if (peek().value() == '{') {
                consume();
                tokens_.push_back({.type = TokenType::OPEN_CURLY});
            } else if (peek().value() == '}') {
                consume();
                tokens_.push_back({.type = TokenType::CLOSE_CURLY});
            } else if (std::isspace(peek().value())) {
                consume();
            } else if (peek().value() == '\0') {
                break;
            } else {
                std::cerr << "You messed up!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        index_ = 0;
    }

    const std::vector<Token>& getTokens() const {
        return tokens_;
    }

private:
    [[nodiscard]] inline std::optional<char> peek(const int offset = 0) const {
        if (index_ + offset >= str_.length()) {
            return {};
        } else {
            return str_.at(index_ + offset);
        }
    }

    inline char consume() {
        return str_.at(index_++);
    }

    std::vector<Token> tokens_;
    std::string        str_;
    size_t             index_;
};

} // namespace deviant

#endif // __LEXER_H__
