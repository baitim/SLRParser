#include "Common/Grammar.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

const int EPS = -1;

Grammar::Grammar() {
    init();
}

void Grammar::init() {
    rules.push_back({nonterm_to_int(Nonterm::GOAL), {nonterm_to_int(Nonterm::PROGRAM)}});
    rules.push_back({nonterm_to_int(Nonterm::PROGRAM), {nonterm_to_int(Nonterm::STATEMENTS)}});
    rules.push_back({nonterm_to_int(Nonterm::STATEMENTS), {nonterm_to_int(Nonterm::STATEMENTS), nonterm_to_int(Nonterm::STATEMENT)}});
    rules.push_back({nonterm_to_int(Nonterm::STATEMENTS), {nonterm_to_int(Nonterm::STATEMENTS), term_to_int(Term::SEMICOLON)}});
    rules.push_back({nonterm_to_int(Nonterm::STATEMENTS), {}});
    rules.push_back({nonterm_to_int(Nonterm::STATEMENT), {nonterm_to_int(Nonterm::EXPR_PLS), term_to_int(Term::SEMICOLON)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_PLS), {nonterm_to_int(Nonterm::EXPR_PLS), term_to_int(Term::PLUS), nonterm_to_int(Nonterm::EXPR_MUL)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_PLS), {nonterm_to_int(Nonterm::EXPR_PLS), term_to_int(Term::MINUS), nonterm_to_int(Nonterm::EXPR_MUL)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_PLS), {nonterm_to_int(Nonterm::EXPR_MUL)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_MUL), {nonterm_to_int(Nonterm::EXPR_MUL), term_to_int(Term::MUL), nonterm_to_int(Nonterm::TERMINAL)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_MUL), {nonterm_to_int(Nonterm::EXPR_MUL), term_to_int(Term::DIV), nonterm_to_int(Nonterm::TERMINAL)}});
    rules.push_back({nonterm_to_int(Nonterm::EXPR_MUL), {nonterm_to_int(Nonterm::TERMINAL)}});
    rules.push_back({nonterm_to_int(Nonterm::TERMINAL), {term_to_int(Term::LPAREN), nonterm_to_int(Nonterm::EXPR_PLS), term_to_int(Term::RPAREN)}});
    rules.push_back({nonterm_to_int(Nonterm::TERMINAL), {term_to_int(Term::NUMBER)}});
    rules.push_back({nonterm_to_int(Nonterm::TERMINAL), {nonterm_to_int(Nonterm::VARIABLE)}});
    rules.push_back({nonterm_to_int(Nonterm::VARIABLE), {term_to_int(Term::ID)}});
    start = nonterm_to_int(Nonterm::GOAL);
}

std::map<int, std::set<int>> Grammar::computeFirst() const {
    std::map<int, std::set<int>> first;
    std::map<int, bool> nullable;

    for (int t = 0; t < static_cast<int>(Term::TERM_COUNT); ++t) {
        first[t].insert(t);
        nullable[t] = false;
    }
    for (int nt = NONTERM_BASE; nt < NONTERM_BASE + static_cast<int>(Nonterm::NONTERM_COUNT); ++nt) {
        first[nt] = {};
        nullable[nt] = false;
    }

    for (const auto& rule : rules) {
        if (rule.rhs.empty()) {
            nullable[rule.lhs] = true;
            first[rule.lhs].insert(EPS);
        }
    }

    bool changed;
    do {
        changed = false;
        for (const auto& rule : rules) {
            if (rule.rhs.empty()) continue;
            int lhs = rule.lhs;
            bool all_nullable = true;
            for (int sym : rule.rhs) {
                for (int t : first[sym]) {
                    if (t != EPS) {
                        if (first[lhs].insert(t).second) changed = true;
                    }
                }
                if (!nullable[sym]) {
                    all_nullable = false;
                    break;
                }
            }
            if (all_nullable) {
                if (first[lhs].insert(EPS).second) changed = true;
                nullable[lhs] = true;
            }
        }
    } while (changed);

    return first;
}

std::vector<std::set<int>> Grammar::computeFollow(const std::map<int, std::set<int>>& first) const {
    int numNonterms = static_cast<int>(Nonterm::NONTERM_COUNT);
    std::vector<std::set<int>> follow(numNonterms);
    follow[static_cast<int>(Nonterm::GOAL)].insert(term_to_int(Term::END));

    std::vector<bool> nullable(numNonterms, false);
    for (int i = 0; i < numNonterms; ++i) {
        int nt = NONTERM_BASE + i;
        if (first.at(nt).count(EPS)) nullable[i] = true;
    }

    bool changed;
    do {
        changed = false;
        for (const auto& rule : rules) {
            int lhs = rule.lhs;
            int lhs_idx = lhs - NONTERM_BASE;
            for (size_t i = 0; i < rule.rhs.size(); ++i) {
                int sym = rule.rhs[i];
                if (!is_nonterm(sym)) continue;
                int sym_idx = sym - NONTERM_BASE;

                std::set<int> first_beta;
                bool beta_nullable = true;
                for (size_t j = i+1; j < rule.rhs.size(); ++j) {
                    int b = rule.rhs[j];
                    for (int t : first.at(b)) {
                        if (t != EPS) first_beta.insert(t);
                    }
                    if (is_nonterm(b)) {
                        int b_idx = b - NONTERM_BASE;
                        if (!nullable[b_idx]) {
                            beta_nullable = false;
                            break;
                        }
                    } else {
                        beta_nullable = false;
                        break;
                    }
                }
                for (int t : first_beta) {
                    if (follow[sym_idx].insert(t).second) changed = true;
                }
                if (beta_nullable || i+1 == rule.rhs.size()) {
                    for (int t : follow[lhs_idx]) {
                        if (follow[sym_idx].insert(t).second) changed = true;
                    }
                }
            }
        }
    } while (changed);

    return follow;
}

} // namespace slr_parser