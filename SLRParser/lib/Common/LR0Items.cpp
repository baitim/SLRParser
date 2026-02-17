#include "Common/LR0Items.hpp"
#include <queue>
#include <set>
#include <map>
#include <vector>

namespace slr_parser {

LR0Automaton::LR0Automaton(const Grammar& g) : grammar(g) {
    build();
}

ItemSet LR0Automaton::closure(const ItemSet& items) {
    ItemSet result = items;
    std::queue<Item> work;
    for (const auto& item : items) work.push(item);
    while (!work.empty()) {
        Item item = work.front(); work.pop();
        const Rule& rule = grammar.getRules()[item.rule_idx];
        if (item.dot >= static_cast<int>(rule.rhs.size())) continue;
        int sym = rule.rhs[item.dot];
        if (!is_nonterm(sym)) continue;
        for (size_t i = 0; i < grammar.getRules().size(); ++i) {
            const Rule& r = grammar.getRules()[i];
            if (r.lhs == sym) {
                Item new_item{static_cast<int>(i), 0};
                if (result.insert(new_item).second) {
                    work.push(new_item);
                }
            }
        }
    }
    return result;
}

ItemSet LR0Automaton::goTo(const ItemSet& items, int symbol) {
    ItemSet next;
    for (const auto& item : items) {
        const Rule& rule = grammar.getRules()[item.rule_idx];
        if (item.dot < static_cast<int>(rule.rhs.size()) && rule.rhs[item.dot] == symbol) {
            next.insert({item.rule_idx, item.dot + 1});
        }
    }
    return closure(next);
}

void LR0Automaton::build() {
    ItemSet start_set;
    start_set.insert({0, 0});
    start_set = closure(start_set);
    states.push_back(start_set);
    state_ids[start_set] = 0;
    std::queue<int> work;
    work.push(0);
    while (!work.empty()) {
        int state_id = work.front(); work.pop();
        std::set<int> symbols;
        for (const auto& item : states[state_id]) {
            const Rule& rule = grammar.getRules()[item.rule_idx];
            if (item.dot < static_cast<int>(rule.rhs.size())) {
                symbols.insert(rule.rhs[item.dot]);
            }
        }
        for (int sym : symbols) {
            ItemSet next_set = goTo(states[state_id], sym);
            if (next_set.empty()) continue;
            int next_id;
            auto it = state_ids.find(next_set);
            if (it == state_ids.end()) {
                next_id = states.size();
                states.push_back(next_set);
                state_ids[next_set] = next_id;
                work.push(next_id);
            } else {
                next_id = it->second;
            }
            transitions[state_id][sym] = next_id;
        }
    }
}

} // namespace slr_parser