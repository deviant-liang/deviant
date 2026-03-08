#ifndef DEVIANT_LEXER_HPP
#define DEVIANT_LEXER_HPP

#include "token.hpp"

#include <functional>
#include <optional>
#include <string_view>

namespace deviant {

struct SourceLocation {
    size_t line = 1;
    size_t column = 1;
};

class Lexer {
public:
    explicit Lexer(const std::string& src);
    ~Lexer();

    auto tokenize() -> void;
    auto get_tokens() const -> const std::vector<Token>&;

private:
    auto setup_rules() -> void;
    auto skip_whitespace() -> void;
    auto advance(size_t len) -> void;
    auto is_eof() const -> bool;
    auto peek() const -> char;
    auto emit(TokenType type, std::string_view val, SourceLocation loc) -> void;

    std::string      str_storage_;
    std::string_view src_;
    size_t           pos_ = 0;
    SourceLocation   cur_loc_;

    std::vector<Token> tokens_;

    struct LexerRule {
        std::function<std::optional<size_t>(std::string_view)> match;
        std::function<void(std::string_view, SourceLocation)>  handle;
    };
    std::vector<LexerRule> rules_;
};

} // namespace deviant

#endif