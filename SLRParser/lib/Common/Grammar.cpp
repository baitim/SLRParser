#include "Common/Grammar.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

namespace {
const Term EPS_TERM = static_cast<Term>(-1);
const Symbol EPS = EPS_TERM;
} // namespace

Grammar::Grammar(std::vector<Rule> r, Nonterm s) : rules(std::move(r)), start(s) {}

const std::vector<Rule>& Grammar::getRules() const { return rules; }
Nonterm Grammar::getStart() const { return start; }

std::map<Symbol, std::set<Term>> Grammar::computeFirst() const {
    std::map<Symbol, std::set<Term>> first;
    std::map<Nonterm, bool> nullable;

    for (int t = 0; t < static_cast<int>(Term::TERM_COUNT); ++t) {
        Term term = static_cast<Term>(t);
        first[term].insert(term);
    }
    for (int nt = 0; nt < static_cast<int>(Nonterm::NONTERM_COUNT); ++nt) {
        Nonterm nonterm = static_cast<Nonterm>(nt);
        first[nonterm] = {};
        nullable[nonterm] = false;
    }

    for (const auto& rule : rules) {
        if (rule.rhs.empty()) {
            nullable[rule.lhs] = true;
            first[rule.lhs].insert(EPS_TERM);
        }
    }

    bool changed;
    do {
        changed = false;
        for (const auto& rule : rules) {
            if (rule.rhs.empty()) continue;
            Nonterm lhs = rule.lhs;
            bool all_nullable = true;
            for (const Symbol& sym : rule.rhs) {
                for (Term t : first[sym]) {
                    if (t != EPS_TERM) {
                        if (first[lhs].insert(t).second) changed = true;
                    }
                }
                if (is_nonterm(sym)) {
                    if (!nullable[as_nonterm(sym)]) {
                        all_nullable = false;
                        break;
                    }
                } else {
                    all_nullable = false;
                    break;
                }
            }
            if (all_nullable) {
                if (first[lhs].insert(EPS_TERM).second) changed = true;
                nullable[lhs] = true;
            }
        }
    } while (changed);

    return first;
}

std::vector<std::set<Term>> Grammar::computeFollow(const std::map<Symbol, std::set<Term>>& first) const {
    int numNonterms = static_cast<int>(Nonterm::NONTERM_COUNT);
    std::vector<std::set<Term>> follow(numNonterms);
    follow[nonterm_to_index(start)].insert(Term::END);

    std::vector<bool> nullable(numNonterms, false);
    for (int i = 0; i < numNonterms; ++i) {
        Nonterm nt = static_cast<Nonterm>(i);
        if (first.at(nt).count(EPS_TERM)) nullable[i] = true;
    }

    bool changed;
    do {
        changed = false;
        for (const auto& rule : rules) {
            Nonterm lhs = rule.lhs;
            int lhs_idx = nonterm_to_index(lhs);
            for (size_t i = 0; i < rule.rhs.size(); ++i) {
                const Symbol& sym = rule.rhs[i];
                if (!is_nonterm(sym)) continue;
                Nonterm B = as_nonterm(sym);
                int B_idx = nonterm_to_index(B);

                std::set<Term> first_beta;
                bool beta_nullable = true;
                for (size_t j = i+1; j < rule.rhs.size(); ++j) {
                    const Symbol& b = rule.rhs[j];
                    for (Term t : first.at(b)) {
                        if (t != EPS_TERM) first_beta.insert(t);
                    }
                    if (is_nonterm(b)) {
                        if (!nullable[nonterm_to_index(as_nonterm(b))]) {
                            beta_nullable = false;
                            break;
                        }
                    } else {
                        beta_nullable = false;
                        break;
                    }
                }
                for (Term t : first_beta) {
                    if (follow[B_idx].insert(t).second) changed = true;
                }
                if (beta_nullable || i+1 == rule.rhs.size()) {
                    for (Term t : follow[lhs_idx]) {
                        if (follow[B_idx].insert(t).second) changed = true;
                    }
                }
            }
        }
    } while (changed);

    return follow;
}

GrammarBuilder::GrammarBuilder() : start(Nonterm::GOAL) {}

GrammarBuilder& GrammarBuilder::addRule(Nonterm lhs, std::initializer_list<Symbol> rhs) {
    rules.push_back({lhs, std::vector<Symbol>(rhs)});
    return *this;
}

GrammarBuilder& GrammarBuilder::setStart(Nonterm s) {
    start = s;
    return *this;
}

Grammar GrammarBuilder::build() const {
    return Grammar(rules, start);
}

} // namespace slr_parser