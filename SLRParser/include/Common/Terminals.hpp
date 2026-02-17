#pragma once

#include <string>
#include <variant>

namespace slr_parser {

enum class Term {
    END, ID, NUMBER, PLUS, MINUS, MUL, DIV, LPAREN, RPAREN, SEMICOLON, TERM_COUNT
};

constexpr std::string_view term_to_string(Term t) {
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

constexpr std::string_view nonterm_to_string(Nonterm n) {
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

using Symbol = std::variant<Term, Nonterm>;

inline bool is_term(const Symbol& s) { return std::holds_alternative<Term>(s); }
inline bool is_nonterm(const Symbol& s) { return std::holds_alternative<Nonterm>(s); }
inline Term as_term(const Symbol& s) { return std::get<Term>(s); }
inline Nonterm as_nonterm(const Symbol& s) { return std::get<Nonterm>(s); }

inline std::string symbol_to_string(const Symbol& s) {
    if (is_term(s)) return std::string(term_to_string(as_term(s)));
    return std::string(nonterm_to_string(as_nonterm(s)));
}

inline int nonterm_to_index(Nonterm n) { return static_cast<int>(n); }
inline int term_to_index(Term t) { return static_cast<int>(t); }

} // namespace slr_parser