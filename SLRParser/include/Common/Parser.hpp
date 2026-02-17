#pragma once

#include "Grammar.hpp"
#include "LR0Items.hpp"
#include "Lexer.hpp"
#include <iosfwd>
#include <stack>
#include <string>
#include <vector>

namespace slr_parser {

struct Action {
    enum Type { SHIFT, REDUCE, ACCEPT, ERROR } type;
    int value;
};

bool parse_program(const std::string& input, std::ostream& out);

class SLRParser {
private:
    Grammar grammar;
    std::vector<std::vector<Action>> actionTable;
    std::vector<std::vector<int>> gotoTable;

private:
    void buildTables(const LR0Automaton& automaton, const std::vector<std::set<int>>& follow);
    std::string ruleToString(const Rule& r) const;

public:
    SLRParser(const Grammar& g, const LR0Automaton& automaton, const std::vector<std::set<int>>& follow);
    bool parse(const std::vector<Token>& tokens, std::ostream& out);
};

} // namespace slr_parser