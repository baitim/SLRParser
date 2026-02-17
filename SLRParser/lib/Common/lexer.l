%option c++ noyywrap yylineno
%option yyclass="slr_parser::Lexer"
%{
#include "Common/Lexer.hpp"
#include "Common/Terminals.hpp"
#define YY_USER_ACTION lexeme = yytext;
%}

NUMBER [1-9][0-9]*|0
ID     [a-zA-Z]+

%%
[ \t\v]+
[\n]              { line++; }
{NUMBER}          { return static_cast<int>(Term::NUMBER); }
{ID}              { return static_cast<int>(Term::ID); }
"+"               { return static_cast<int>(Term::PLUS); }
"-"               { return static_cast<int>(Term::MINUS); }
"*"               { return static_cast<int>(Term::MUL); }
"/"               { return static_cast<int>(Term::DIV); }
"("               { return static_cast<int>(Term::LPAREN); }
")"               { return static_cast<int>(Term::RPAREN); }
";"               { return static_cast<int>(Term::SEMICOLON); }
<<EOF>>           { return static_cast<int>(Term::END); }
.                 { return static_cast<int>(Term::END); }
%%