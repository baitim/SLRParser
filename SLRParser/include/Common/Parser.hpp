#pragma once

#include "Grammar.hpp"
#include "LR0Items.hpp"
#include "Lexer.hpp"
#include <iosfwd>
#include <memory>
#include <optional>
#include <span>
#include <stack>
#include <string>
#include <vector>

namespace slr_parser {

struct Action {
    enum Type { SHIFT, REDUCE, ACCEPT, ERROR } type;
    int value; // for SHIFT: next state; for REDUCE: rule index
};

class Parser {
public:
    virtual ~Parser() = default;
    virtual bool parse(std::span<const Token> tokens, std::ostream& out) = 0;
};

class SLRParser final : public Parser {
private:
    Grammar grammar;
    std::vector<std::vector<Action>> actionTable;
    std::vector<std::vector<std::optional<int>>> gotoTable;

private:
    void buildTables(const LR0Automaton& automaton, const std::vector<std::set<Term>>& follow);
    std::string ruleToString(const Rule& r) const;

public:
    SLRParser(const Grammar& g, const LR0Automaton& automaton, const std::vector<std::set<Term>>& follow);
    bool parse(std::span<const Token> tokens, std::ostream& out) override;
};

bool parse_program(const std::string& input, std::ostream& out);

} // namespace slr_parser