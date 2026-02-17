#pragma once

#include "Terminals.hpp"
#include <map>
#include <set>
#include <vector>

namespace slr_parser {

struct Rule {
    int lhs;
    std::vector<int> rhs;
};

class Grammar {
private:
    std::vector<Rule> rules;
    int start;

private:
    void init();

public:
    Grammar();
    const std::vector<Rule>& getRules() const { return rules; }
    int getStart() const { return start; }
    std::map<int, std::set<int>> computeFirst() const;
    std::vector<std::set<int>> computeFollow(const std::map<int, std::set<int>>& first) const;
};

} // namespace slr_parser