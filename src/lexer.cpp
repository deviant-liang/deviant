#include "lexer.hpp"

#include <iostream>
#include <regex>
#include <unordered_map>
#include <vector>

namespace deviant {

Lexer::Lexer(const std::string& src)
    : str_storage_(src), src_(str_storage_), pos_(0) {
    setup_rules();
}

Lexer::~Lexer() = default;

auto Lexer::get_tokens() const -> const std::vector<Token>& { return tokens_; }

auto Lexer::tokenize() -> void {
    tokens_.clear();
    while (!is_eof()) {
        skip_whitespace();
        if (is_eof()) break;

        const std::string_view remaining = src_.substr(pos_);
        const SourceLocation   start_loc = cur_loc_;
        bool                   matched = false;

        for (const auto& rule : rules_) {
            if (auto length = rule.match(remaining)) {
                rule.handle(remaining.substr(0, *length), start_loc);
                advance(*length);
                matched = true;
                break;
            }
        }

        if (!matched) {
            std::cerr << "Unexpected character at " << cur_loc_.line << ":"
                      << cur_loc_.column << "\n";
            advance(1);
        }
    }
}

auto Lexer::setup_rules() -> void {
    rules_.push_back(
        {[](std::string_view s) -> std::optional<size_t> {
             static const std::regex re{R"(^[a-zA-Z_][a-zA-Z0-9_]*)"};
             std::cmatch             m;
             if (std::regex_search(s.data(), s.data() + s.size(), m, re))
                 return m.length();
             return std::nullopt;
         },
         [this](std::string_view text, SourceLocation loc) {
             static const std::unordered_map<std::string_view, TokenType>
                 kKeywords = {
                     {"ret", TokenType::RETURN}, {"var", TokenType::VAR},
                     {"if", TokenType::IF},      {"else", TokenType::ELSE},
                     {"fn", TokenType::FN},      {"int", TokenType::INT}};
             auto it = kKeywords.find(text);
             emit(it != kKeywords.end() ? it->second : TokenType::IDENTIFIER,
                  text, loc);
         }});

    auto add_rule = [this](const std::string& pattern, TokenType type) {
        rules_.push_back(
            {[pattern](std::string_view s) -> std::optional<size_t> {
                 std::regex  re{pattern};
                 std::cmatch m;
                 if (std::regex_search(s.data(), s.data() + s.size(), m, re))
                     return m.length();
                 return std::nullopt;
             },
             [this, type](std::string_view text, SourceLocation loc) {
                 emit(type, text, loc);
             }});
    };

    add_rule(R"(^[0-9]+)", TokenType::INT_LIT);
    add_rule(R"(^->)", TokenType::FN_TYPE);
    add_rule(R"(^==)", TokenType::EQ);
    add_rule(R"(^!=)", TokenType::NE);
    add_rule(R"(^<=)", TokenType::LE);
    add_rule(R"(^>=)", TokenType::GE);
    add_rule(R"(^\()", TokenType::OPEN_PAREN);
    add_rule(R"(^\))", TokenType::CLOSE_PAREN);
    add_rule(R"(^\{)", TokenType::OPEN_CURLY);
    add_rule(R"(^\})", TokenType::CLOSE_CURLY);
    add_rule(R"(^=)", TokenType::ASSIGNMENT);
    add_rule(R"(^;)", TokenType::SEMICOLON);
    add_rule(R"(^\+)", TokenType::PLUS);
    add_rule(R"(^-)", TokenType::MINUS);
    add_rule(R"(^\*)", TokenType::STAR);
    add_rule(R"(^/)", TokenType::FSLASH);
    add_rule(R"(^,)", TokenType::COMMA);
    add_rule(R"(^<)", TokenType::LT);
    add_rule(R"(^>)", TokenType::GT);
    add_rule(R"(^!)", TokenType::EXCLAMATION);
}

auto Lexer::advance(size_t len) -> void {
    for (size_t i = 0; i < len; ++i) {
        if (src_[pos_] == '\n') {
            cur_loc_.line++;
            cur_loc_.column = 1;
        } else {
            cur_loc_.column++;
        }
        pos_++;
    }
}

auto Lexer::skip_whitespace() -> void {
    while (pos_ < src_.size()) {
        if (std::isspace(src_[pos_])) {
            advance(1);
        } else if (src_.substr(pos_, 2) == "//") {
            while (pos_ < src_.size() && src_[pos_] != '\n')
                advance(1);
        } else
            break;
    }
}

auto Lexer::is_eof() const -> bool { return pos_ >= src_.size(); }

auto Lexer::peek() const -> char { return is_eof() ? '\0' : src_[pos_]; }

auto Lexer::emit(TokenType type, std::string_view val,
                 SourceLocation loc) -> void {
    Token tok;
    tok.type = type;
    tok.value = std::string(val);
    tokens_.push_back(tok);
}

} // namespace deviant