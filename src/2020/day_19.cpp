#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>
#include <stack>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    class rule;
    using rule_tbl = std::unordered_map<int, std::shared_ptr<rule>>;

    class rule {
    public:
        virtual std::vector<int> validate(const rule_tbl& rules, const std::string& inp, int pos) const = 0;
    };

    using rule_ptr = std::shared_ptr<rule>;

    class literal_rule : public rule {
        char letter_;
    public:
        literal_rule(char letter) : letter_(letter) {
        }

        std::vector<int> validate(const rule_tbl& rules, const std::string& inp, int pos) const override {
            if (pos >= inp.size()) {
                return {};
            }
            if (inp[pos] != letter_) {
                return {};
            }
            return std::vector<int>{ pos + 1 };
        }
    };

    class conjunction_rule : public rule {
        std::vector<int> children_;
    public:
        conjunction_rule(const std::vector<int>& child_rules) :
            children_(child_rules) {
        }

        std::vector<int> validate(const rule_tbl& rules, const std::string& inp, int pos) const override {
            struct state {
                int child;
                int pos;
            };

            if (pos >= inp.size()) {
                return {};
            }

            std::unordered_set<int> results;
            std::stack<state> stack;
            stack.push({ 0,pos });
            while (!stack.empty()) {
                state s = stack.top();
                stack.pop();
                if (s.child < children_.size()) {
                    auto new_positions = rules.at(children_[s.child])->validate(rules, inp, s.pos);
                    for (int new_pos : new_positions) {
                        stack.push({ s.child + 1, new_pos });
                    }
                } else {
                    results.insert(s.pos);
                }
            }

            return results | r::to<std::vector<int>>();
        }
    };

    class disjunction_rule : public rule {
        std::vector<rule_ptr> children_;
    public:
        disjunction_rule(const std::vector<int>& lhs, const std::vector<int>& rhs) {
            children_.push_back(std::make_shared<conjunction_rule>(lhs));
            children_.push_back(std::make_shared<conjunction_rule>(rhs));
        }

        std::vector<int> validate(const rule_tbl& rules, const std::string& inp, int pos) const override {
            if (pos >= inp.size()) {
                return {};
            }
            std::unordered_set<int> results;
            for (auto child : children_) {
                auto new_positions = child->validate(rules, inp, pos);
                if (!new_positions.empty()) {
                    for (int new_pos : new_positions) {
                        results.insert(new_pos);
                    }
                }
            }
            return results | r::to<std::vector<int>>();
        }
    };

    rule_ptr parse_literal(const std::string& str) {
        auto letter = aoc::extract_alphabetic(str);
        return std::make_shared<literal_rule>(letter.front().front());
    }

    rule_ptr parse_conjunction(const std::string& str) {
        auto nums = aoc::extract_numbers(str);
        return std::make_shared<conjunction_rule>(nums);
    }

    rule_ptr parse_disjunction(const std::string& str) {
        auto [lhs,rhs] = aoc::split_to_tuple<2>(str, '|');
        return std::make_shared<disjunction_rule>(
            aoc::extract_numbers(lhs), 
            aoc::extract_numbers(rhs)
       );
    }

    rule_tbl::value_type parse_rule(const std::string& str) {
        auto [rule_id_txt, rule_txt] = aoc::split_to_tuple<2>(str, ':');
        rule_txt = aoc::trim(rule_txt);

        int rule_id = std::stoi(rule_id_txt);
        rule_ptr rule = nullptr;
        if (rule_txt.contains('"')) {
            rule = parse_literal(rule_txt);
        } else if (rule_txt.contains('|')) {
            rule = parse_disjunction(rule_txt);
        } else {
            rule = parse_conjunction(rule_txt);
        }
        return { rule_id, rule };
    }

    rule_tbl parse_rules(const std::vector<std::string>& input) {
        return input | rv::transform(parse_rule) | r::to<rule_tbl>();
    }

    std::tuple<rule_tbl, std::vector<std::string>> parse_input(
        const std::vector<std::string>& input) {
        auto groups = aoc::group_strings_separated_by_blank_lines(input);
        return {
            parse_rules(groups.front()),
            groups.back()
        };
    }

    bool is_match(const rule_tbl& rules, rule_ptr rule, const std::string& inp) {
        auto result = rule->validate(rules, inp, 0);
        if (result.empty()) {
            return false;
        }
        if (r::find(result, inp.size()) == result.end()) {
            return false;
        }

        return true;
    }

    rule_tbl new_rules(const rule_tbl& rules) {
        auto new_rules = rules;
        new_rules[8] = std::get<1>(parse_rule("8: 42 | 42 8"));
        new_rules[11] = std::get<1>(parse_rule("11: 42 31 | 42 11 31"));

        return new_rules;
    }

    int count_valid_messages(const rule_tbl& rules, const std::vector<std::string>& messages) {
        return r::count_if(messages,
            [&rules](auto&& str)->int {
                return is_match(rules, rules.at(0), str);
            }
        );
    }
}

void aoc::y2020::day_19(const std::string& title) {
    
    auto [rules, messages] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2020, 19))
    );
    
    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", count_valid_messages( rules, messages ));
    std::println("  part 2: {}", count_valid_messages( new_rules(rules), messages ));
    
}