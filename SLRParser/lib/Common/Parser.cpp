#include "Common/Parser.hpp"
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

namespace slr_parser {

SLRParser::SLRParser(const Grammar& g, const LR0Automaton& automaton, const std::vector<std::set<int>>& follow)
    : grammar(g) {
    buildTables(automaton, follow);
}

void SLRParser::buildTables(const LR0Automaton& automaton, const std::vector<std::set<int>>& follow) {
    int numStates = automaton.getStates().size();
    int numTerms = static_cast<int>(Term::TERM_COUNT);
    int numNonterms = static_cast<int>(Nonterm::NONTERM_COUNT);
    actionTable.resize(numStates, std::vector<Action>(numTerms, {Action::ERROR, 0}));
    gotoTable.resize(numStates, std::vector<int>(numNonterms, -1));

    const auto& states = automaton.getStates();
    const auto& trans = automaton.getTransitions();

    for (int i = 0; i < numStates; ++i) {
        auto it_state = trans.find(i);
        if (it_state != trans.end()) {
            for (const auto& p : it_state->second) {
                int sym = p.first;
                if (is_term(sym)) {
                    actionTable[i][sym] = {Action::SHIFT, p.second};
                } else {
                    int nonterm = sym - NONTERM_BASE;
                    gotoTable[i][nonterm] = p.second;
                }
            }
        }
        for (const auto& item : states[i]) {
            const Rule& rule = grammar.getRules()[item.rule_idx];
            if (item.dot == static_cast<int>(rule.rhs.size())) {
                if (rule.lhs == grammar.getStart()) {
                    actionTable[i][term_to_int(Term::END)] = {Action::ACCEPT, 0};
                } else {
                    int lhs_idx = rule.lhs - NONTERM_BASE;
                    for (int t : follow[lhs_idx]) {
                        if (actionTable[i][t].type == Action::ERROR) {
                            actionTable[i][t] = {Action::REDUCE, item.rule_idx};
                        }
                    }
                }
            }
        }
    }
}

std::string SLRParser::ruleToString(const Rule& r) const {
    std::string lhs = nonterm_to_string(int_to_nonterm(r.lhs));
    std::string rhs;
    for (int sym : r.rhs) {
        if (is_term(sym)) rhs += term_to_string(int_to_term(sym));
        else rhs += nonterm_to_string(int_to_nonterm(sym));
    }
    return lhs + "->" + rhs;
}

bool SLRParser::parse(const std::vector<Token>& tokens, std::ostream& out) {
    std::stack<int> state_stack;
    std::stack<std::string> symbol_stack;
    state_stack.push(0);
    size_t pos = 0;
    out << "STACK\tINPUT\tACTION\n";
    while (true) {
        Term current_term = (pos < tokens.size()) ? tokens[pos].type : Term::END;
        int cur_tok = term_to_int(current_term);
        int state = state_stack.top();
        Action act = actionTable[state][cur_tok];
        std::string stack_str;
        std::stack<std::string> temp = symbol_stack;
        std::vector<std::string> syms;
        while (!temp.empty()) {
            syms.push_back(temp.top());
            temp.pop();
        }
        for (auto it = syms.rbegin(); it != syms.rend(); ++it) {
            stack_str += *it;
        }
        std::string input_str;
        for (size_t i = pos; i < tokens.size(); ++i) {
            input_str += tokens[i].lexeme;
        }
        input_str += "$";
        std::string action_str;
        switch (act.type) {
            case Action::SHIFT:
                action_str = "Shift";
                break;
            case Action::REDUCE: {
                const Rule& r = grammar.getRules()[act.value];
                action_str = "Reduce " + ruleToString(r);
                break;
            }
            case Action::ACCEPT:
                action_str = "Accept";
                break;
            case Action::ERROR:
                action_str = "Error";
                break;
        }
        out << stack_str << "\t" << input_str << "\t" << action_str << "\n";
        if (act.type == Action::ACCEPT) return true;
        if (act.type == Action::ERROR) return false;
        if (act.type == Action::SHIFT) {
            state_stack.push(act.value);
            symbol_stack.push(tokens[pos].lexeme);
            ++pos;
        } else if (act.type == Action::REDUCE) {
            const Rule& r = grammar.getRules()[act.value];
            int pop_count = r.rhs.size();
            for (int i = 0; i < pop_count; ++i) {
                state_stack.pop();
                symbol_stack.pop();
            }
            int new_state = gotoTable[state_stack.top()][r.lhs - NONTERM_BASE];
            state_stack.push(new_state);
            symbol_stack.push(nonterm_to_string(int_to_nonterm(r.lhs)));
        }
    }
}

bool parse_program(const std::string& input, std::ostream& out) {
    std::istringstream iss(input);
    Lexer lexer(&iss);
    std::vector<Token> tokens;
    while (true) {
        Token t = lexer.nextToken();
        tokens.push_back(t);
        if (t.type == Term::END) break;
    }
    Grammar grammar;
    auto first = grammar.computeFirst();
    auto follow = grammar.computeFollow(first);
    LR0Automaton automaton(grammar);
    SLRParser parser(grammar, automaton, follow);
    return parser.parse(tokens, out);
}

} // namespace slr_parser