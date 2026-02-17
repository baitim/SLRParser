#pragma once

#include <string>

namespace slr_parser {

enum class Term {
    END, ID, NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, SEMICOLON, TERM_COUNT
};

inline std::string term_to_string(Term t) {
    switch (t) {
        case Term::END: return "$";
        case Term::ID: return "id";
        case Term::NUMBER: return "num";
        case Term::PLUS: return "+";
        case Term::MINUS: return "-";
        case Term::MUL: return "*";
        case Term::DIV: return "/";
        case Term::LPAREN: return "(";
        case Term::RPAREN: return ")";
        case Term::SEMICOLON: return ";";
        default: return "";
    }
}

enum class Nonterm {
    GOAL, PROGRAM, STATEMENTS, STATEMENT, EXPR_PLS, EXPR_MUL, TERMINAL, VARIABLE, NONTERM_COUNT
};

inline std::string nonterm_to_string(Nonterm n) {
    switch (n) {
        case Nonterm::GOAL: return "S'";
        case Nonterm::PROGRAM: return "program";
        case Nonterm::STATEMENTS: return "statements";
        case Nonterm::STATEMENT: return "statement";
        case Nonterm::EXPR_PLS: return "expr_pls";
        case Nonterm::EXPR_MUL: return "expr_mul";
        case Nonterm::TERMINAL: return "terminal";
        case Nonterm::VARIABLE: return "variable";
        default: return "";
    }
}

const int TERM_BASE = 0;
const int NONTERM_BASE = static_cast<int>(Term::TERM_COUNT);

inline bool is_term(int sym) { return sym < NONTERM_BASE; }
inline bool is_nonterm(int sym) { return sym >= NONTERM_BASE; }
inline int term_to_int(Term t) { return static_cast<int>(t); }
inline int nonterm_to_int(Nonterm n) { return NONTERM_BASE + static_cast<int>(n); }
inline Term int_to_term(int sym) { return static_cast<Term>(sym); }
inline Nonterm int_to_nonterm(int sym) { return static_cast<Nonterm>(sym - NONTERM_BASE); }

} // namespace slr_parser