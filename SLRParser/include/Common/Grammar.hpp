#pragma once

#include "Terminals.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

struct Rule {
    Nonterm lhs;
    std::vector<Symbol> rhs;
};

class Grammar {
private:
    std::vector<Rule> rules;
    Nonterm start;

public:
    Grammar(std::vector<Rule> r, Nonterm s);
    const std::vector<Rule>& getRules() const;
    Nonterm getStart() const;
    std::map<Symbol, std::set<Term>> computeFirst() const;
    std::vector<std::set<Term>> computeFollow(const std::map<Symbol, std::set<Term>>& first) const;
};

class GrammarBuilder {
private:
    std::vector<Rule> rules;
    Nonterm start;

public:
    GrammarBuilder();
    GrammarBuilder& addRule(Nonterm lhs, std::initializer_list<Symbol> rhs);
    GrammarBuilder& setStart(Nonterm s);
    Grammar build() const;
};

} // namespace slr_parser