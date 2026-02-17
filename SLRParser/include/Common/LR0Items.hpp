#pragma once

#include "Grammar.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

struct Item {
    int rule_idx;
    int dot;

    bool operator==(const Item& other) const;
    bool operator<(const Item& other) const;
};

using ItemSet = std::set<Item>;

class LR0Automaton {
private:
    Grammar grammar;
    std::vector<ItemSet> states;
    std::map<int, std::map<Symbol, int>> transitions;
    std::map<ItemSet, int> state_ids;

private:
    ItemSet closure(const ItemSet& items) const;
    ItemSet goTo(const ItemSet& items, const Symbol& symbol) const;
    void build();

public:
    LR0Automaton(const Grammar& g);
    const std::vector<ItemSet>& getStates() const;
    const std::map<int, std::map<Symbol, int>>& getTransitions() const;
};

} // namespace slr_parser