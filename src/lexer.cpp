#include "lexer.hpp"
#include "token.hpp"

namespace deviant {
Lexer::Lexer(const std::string& src) : index_(0), str_(src) {
}

} // namespace deviant