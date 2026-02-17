#pragma once

#include "Terminals.hpp"
#include <string>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

namespace slr_parser {

struct Token {
    Term type;
    std::string lexeme;
    int line;
};

class Lexer : public yyFlexLexer {
private:
    std::string lexeme;
    int line;

private:
    int yylex() override;

public:
    Lexer(std::istream* in);
    Token nextToken();
};

} // namespace slr_parser