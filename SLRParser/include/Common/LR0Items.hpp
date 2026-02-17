#pragma once

#include "Grammar.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

struct Item {
    int rule_idx;
    int dot;

    bool operator==(const Item& other) const { return rule_idx == other.rule_idx && dot == other.dot; }
    bool operator<(const Item& other) const {
        if (rule_idx != other.rule_idx) return rule_idx < other.rule_idx;
        return dot < other.dot;
    }
};

using ItemSet = std::set<Item>;

class LR0Automaton {
private:
    Grammar grammar;
    std::vector<ItemSet> states;
    std::map<int, std::map<int, int>> transitions;
    std::map<ItemSet, int> state_ids;

private:
    ItemSet closure(const ItemSet& items);
    ItemSet goTo(const ItemSet& items, int symbol);
    void build();

public:
    LR0Automaton(const Grammar& g);
    const std::vector<ItemSet>& getStates() const { return states; }
    const std::map<int, std::map<int, int>>& getTransitions() const { return transitions; }
};

} // namespace slr_parser