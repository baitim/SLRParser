#include "Common/Parser.hpp"
#include "Common/Error.hpp"
#include <format>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

namespace slr_parser {

SLRParser::SLRParser(const Grammar& g, const LR0Automaton& automaton, const std::vector<std::set<Term>>& follow)
    : grammar(g) {
    buildTables(automaton, follow);
}

void SLRParser::buildTables(const LR0Automaton& automaton, const std::vector<std::set<Term>>& follow) {
    int numStates = automaton.getStates().size();
    int numTerms = static_cast<int>(Term::TERM_COUNT);
    int numNonterms = static_cast<int>(Nonterm::NONTERM_COUNT);
    actionTable.resize(numStates, std::vector<Action>(numTerms, {Action::ERROR, 0}));
    gotoTable.resize(numStates, std::vector<std::optional<int>>(numNonterms, std::nullopt));

    const auto& states = automaton.getStates();
    const auto& trans = automaton.getTransitions();

    for (int i = 0; i < numStates; ++i) {
        auto it_state = trans.find(i);
        if (it_state != trans.end()) {
            for (const auto& [sym, next] : it_state->second) {
                if (is_term(sym)) {
                    actionTable[i][term_to_index(as_term(sym))] = {Action::SHIFT, next};
                } else {
                    gotoTable[i][nonterm_to_index(as_nonterm(sym))] = next;
                }
            }
        }
        for (const auto& item : states[i]) {
            const Rule& rule = grammar.getRules()[item.rule_idx];
            if (item.dot == static_cast<int>(rule.rhs.size())) {
                if (rule.lhs == grammar.getStart()) {
                    actionTable[i][term_to_index(Term::END)] = {Action::ACCEPT, 0};
                } else {
                    int lhs_idx = nonterm_to_index(rule.lhs);
                    for (Term t : follow[lhs_idx]) {
                        int t_idx = term_to_index(t);
                        if (actionTable[i][t_idx].type == Action::ERROR) {
                            actionTable[i][t_idx] = {Action::REDUCE, item.rule_idx};
                        }
                    }
                }
            }
        }
    }
}

std::string SLRParser::ruleToString(const Rule& r) const {
    std::string lhs(nonterm_to_string(r.lhs));
    std::string rhs;
    for (const auto& sym : r.rhs) {
        rhs += symbol_to_string(sym);
    }
    return std::format("{}->{}", lhs, rhs);
}

bool SLRParser::parse(std::span<const Token> tokens, std::ostream& out) {
    std::stack<int> state_stack;
    std::stack<Symbol> symbol_stack;
    state_stack.push(0);
    size_t pos = 0;
    out << "STACK\tINPUT\tACTION\n";
    while (true) {
        Term current_term = (pos < tokens.size()) ? tokens[pos].type : Term::END;
        int cur_tok = term_to_index(current_term);
        int state = state_stack.top();
        Action act = actionTable[state][cur_tok];
        std::string stack_str;
        std::stack<Symbol> temp = symbol_stack;
        std::vector<std::string> syms;
        while (!temp.empty()) {
            syms.push_back(symbol_to_string(temp.top()));
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
        if (act.type == Action::ERROR) throw slr_parser::ParseError("Parse error");
        if (act.type == Action::SHIFT) {
            state_stack.push(act.value);
            symbol_stack.push(tokens[pos].type);
            ++pos;
        } else if (act.type == Action::REDUCE) {
            const Rule& r = grammar.getRules()[act.value];
            int pop_count = r.rhs.size();
            for (int i = 0; i < pop_count; ++i) {
                state_stack.pop();
                symbol_stack.pop();
            }
            int new_state = *gotoTable[state_stack.top()][nonterm_to_index(r.lhs)];
            state_stack.push(new_state);
            symbol_stack.push(r.lhs);
        }
    }
    return false;
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

    GrammarBuilder builder;
    builder.setStart(Nonterm::GOAL)
        .addRule(Nonterm::GOAL, {Nonterm::PROGRAM})
        .addRule(Nonterm::PROGRAM, {Nonterm::STATEMENTS})
        .addRule(Nonterm::STATEMENTS, {Nonterm::STATEMENTS, Nonterm::STATEMENT})
        .addRule(Nonterm::STATEMENTS, {Nonterm::STATEMENTS, Term::SEMICOLON})
        .addRule(Nonterm::STATEMENTS, {})
        .addRule(Nonterm::STATEMENT, {Nonterm::EXPR_PLS, Term::SEMICOLON})
        .addRule(Nonterm::EXPR_PLS, {Nonterm::EXPR_PLS, Term::PLUS, Nonterm::EXPR_MUL})
        .addRule(Nonterm::EXPR_PLS, {Nonterm::EXPR_PLS, Term::MINUS, Nonterm::EXPR_MUL})
        .addRule(Nonterm::EXPR_PLS, {Nonterm::EXPR_MUL})
        .addRule(Nonterm::EXPR_MUL, {Nonterm::EXPR_MUL, Term::MUL, Nonterm::TERMINAL})
        .addRule(Nonterm::EXPR_MUL, {Nonterm::EXPR_MUL, Term::DIV, Nonterm::TERMINAL})
        .addRule(Nonterm::EXPR_MUL, {Nonterm::TERMINAL})
        .addRule(Nonterm::TERMINAL, {Term::LPAREN, Nonterm::EXPR_PLS, Term::RPAREN})
        .addRule(Nonterm::TERMINAL, {Term::NUMBER})
        .addRule(Nonterm::TERMINAL, {Nonterm::VARIABLE})
        .addRule(Nonterm::VARIABLE, {Term::ID});
    Grammar grammar = builder.build();

    auto first = grammar.computeFirst();
    auto follow = grammar.computeFollow(first);
    LR0Automaton automaton(grammar);
    SLRParser parser(grammar, automaton, follow);
    return parser.parse(tokens, out);
}

} // namespace slr_parser