#include "Common/Lexer.hpp"

namespace slr_parser {

Lexer::Lexer(std::istream* in) : yyFlexLexer(in), line(1) {}

Token Lexer::nextToken() {
    int type = yylex();
    return { static_cast<Term>(type), lexeme, line };
}

} // namespace slr_parser