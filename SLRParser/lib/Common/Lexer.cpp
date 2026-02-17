#include "Common/Lexer.hpp"

namespace slr_parser {

Token Lexer::nextToken() {
    int type = yylex();
    return { static_cast<Term>(type), lexeme, line };
}

} // namespace slr_parser