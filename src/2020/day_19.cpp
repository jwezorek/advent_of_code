#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <memory>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    class rule;
    using rule_tbl = std::unordered_map<int, std::shared_ptr<rule>>;

    class rule {
    protected:
        std::vector<int> child_rules_;
    public:
        rule(const std::vector<int>& child_rules) : child_rules_(child_rules) {
        }
        virtual void populate(const rule_tbl& rules) = 0;
        virtual int check() const = 0;
        virtual std::optional<int> validate(const std::string& inp, int pos) const = 0;
    };

    using rule_ptr = std::shared_ptr<rule>;

    class literal_rule : public rule {
        char letter_;
    public:
        literal_rule(char letter) : rule(std::vector<int>{}), letter_(letter) {
        }

        void populate(const rule_tbl& rules) override {
        }

        int check() const override {
            return 1;
        }

        std::optional<int> validate(const std::string& inp, int pos) const override {
            if (pos >= inp.size()) {
                return {};
            }
            if (inp[pos] != letter_) {
                return {};
            }
            return pos + 1;
        }
    };


    class conjunction_rule : public rule {
        std::vector<rule_ptr> children_;
    public:
        conjunction_rule(const std::vector<int>& child_rules) :
            rule(child_rules) {
        }

        conjunction_rule(rule_ptr first, rule_ptr second) :
            rule({}) {
            children_ = { first, second };
        }

        void populate(const rule_tbl& rules) override {
            children_ = child_rules_ | rv::transform(
                [&rules](int index) {
                    return rules.at(index);
                }
            ) | r::to<std::vector<rule_ptr>>();
        }

        std::optional<int> validate(const std::string& inp, int pos) const override {
            if (pos >= inp.size()) {
                return {};
            }
            for (auto child : children_) {
                auto new_pos = child->validate(inp, pos);
                if (!new_pos) {
                    return {};
                }
                pos = *new_pos;
            }
            return pos;
        }

        int check() const override {
            auto checks = children_ | rv::transform(
                [](auto child) {return child->check(); }
            );
            if (r::find(checks, -1) != checks.end()) {
                return -1;
            }
            return r::fold_left(checks, 0, std::plus<>());
        }
    };

    class disjunction_rule : public rule {
        std::vector<rule_ptr> children_;
    public:
        disjunction_rule(const std::vector<int>& child_rules) :
            rule(child_rules) {
        }

        void populate(const rule_tbl& rules) override {
            auto subrules = child_rules_ | rv::transform(
                [&rules](int index) {
                    return rules.at(index);
                }
            ) | r::to<std::vector<rule_ptr>>();
            rule_ptr lhs, rhs;
            if (subrules.size() == 4) {
                lhs = std::make_shared<conjunction_rule>(subrules[0], subrules[1]);
                rhs = std::make_shared<conjunction_rule>(subrules[2], subrules[3]);
            } else if (subrules.size() == 2) {
                lhs = subrules[0];
                rhs = subrules[1];
            } else {
                throw std::runtime_error("invalid disjunction rule");
            }
            children_ = { lhs, rhs };
        }

        int check() const override {
            auto a = children_.front()->check();
            auto b = children_.back()->check();
            if (a == -1 || b == -1) {
                return -1;
            }
            return (a == b) ? a : -1;
        }

        std::optional<int> validate(const std::string& inp, int pos) const override {
            if (pos >= inp.size()) {
                return {};
            }
            for (auto child : children_) {
                auto new_pos = child->validate(inp, pos);
                if (new_pos) {
                    return new_pos;
                }
            }
            return {};
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
        auto nums = aoc::extract_numbers(str);
        return std::make_shared<disjunction_rule>(nums);
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
        auto rules = input | rv::transform(parse_rule) | r::to<rule_tbl>();
        for (auto rule : rules | rv::values) {
            rule->populate(rules);
        }
        return rules;
    }

    std::tuple<rule_tbl, std::vector<std::string>> parse_input(
        const std::vector<std::string>& input) {
        auto groups = aoc::group_strings_separated_by_blank_lines(input);
        return {
            parse_rules(groups.front()),
            groups.back()
        };
    }

    bool is_match(rule_ptr rule, const std::string& inp) {
        auto result = rule->validate(inp, 0);
        if (!result) {
            return false;
        }
        if (*result != inp.size()) {
            return false;
        }

        return true;
    }

    int do_part_2(const rule_tbl& rules, const std::vector<std::string>& mesages) {
        return 0;
    }
}

void aoc::y2020::day_19(const std::string& title) {
    
    auto [rules, messages] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2020, 19))
    );

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            messages | rv::transform(
                [&rules](auto&& str)->int {
                    return is_match(rules.at(0), str) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}", do_part_2(rules, messages));
    
}