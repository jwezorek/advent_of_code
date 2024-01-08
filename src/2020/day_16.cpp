#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using ticket = std::vector<int>;

    struct range {
        int from;
        int to;
    };

    struct rule {
        std::string field;
        range rng1;
        range rng2;
    };

    rule parse_rule(const std::string& str) {
        auto [field, ranges] = aoc::split_to_tuple<2>(str, ':');
        auto nums = aoc::extract_numbers(ranges);
        return {
            field,
            {nums[0], nums[1]},
            {nums[2], nums[3]}
        };
    }

    std::tuple<std::vector<rule>, ticket, std::vector<ticket>> parse(
            const std::vector<std::string>& inp) {
        auto pieces = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            pieces[0] | rv::transform(parse_rule) | r::to<std::vector<rule>>(),
            aoc::extract_numbers(pieces[1][1]),
            pieces[2] | rv::drop(1) | rv::transform(
                    [](const std::string& str) {
                        return aoc::extract_numbers(str);
                     }
                ) | r::to<std::vector<ticket>>()
        };
    }

    bool is_in_range(int val, const range& rng) {
        return val >= rng.from && val <= rng.to;
    }

    bool is_valid_value(int value, const rule& rule) {
        return is_in_range(value, rule.rng1) || is_in_range(value, rule.rng2);
    }

    bool valid_in_some_field(int value, const std::vector<rule>& rules) {
        for (const auto& rule : rules) {
            if (is_valid_value(value, rule)) {
                return true;
            }
        }
        return false;
    }

    std::optional<int> not_valid_value(const ticket& ticket, const std::vector<rule>& rules) {
        for (auto val : ticket) {
            if (!valid_in_some_field(val, rules)) {
                return val;
            }
        }
        return {};
    }

    std::vector<ticket> valid_tickets(const std::vector<rule>& rules,
            const std::vector<ticket>& all_tickets) {
        return all_tickets | rv::filter(
            [&rules](auto&& ticket) {
                return !not_valid_value(ticket, rules).has_value();
            }
        ) | r::to<std::vector<ticket>>();
    }

    bool all_valid(range rng1, range rng2, auto nums) {
        return r::all_of(nums,
            [rng1,rng2](int num) {
                return is_in_range(num, rng1) || is_in_range(num, rng2);
            }
        );
    }

    std::optional<int> find_field_index(range rng1, range rng2, 
            const std::vector<ticket>& tickets,
            const std::unordered_set<int>& fields) {
        std::vector<int> possible_indices;
        for (int i : fields) {
            auto field_values = tickets | rv::transform(
                [i](const auto& ticket) { return ticket[i]; }
            );
            if (all_valid(rng1, rng2, field_values)) {
                possible_indices.push_back(i);
            }
        }
        if (possible_indices.size() == 1) {
            return possible_indices.front();
        }
        return {};
    }

    std::tuple<std::string, int> find_one_field(const std::vector<rule>& rules,
            const std::vector<ticket>& tickets,
            const std::unordered_set<int>& fields) {
        for (const auto& rule : rules) {
            auto maybe_index = find_field_index(rule.rng1, rule.rng2, tickets, fields);
            if (maybe_index) {
                return { rule.field, maybe_index.value()};
            }
        }
        throw std::runtime_error("ambiguous input");
    }

    std::unordered_map<std::string, int> find_field_to_index_mapping(
            const std::vector<rule>& rules, const std::vector<ticket>& all_tickets) {

        auto tickets = valid_tickets(rules, all_tickets);
        std::unordered_map<std::string, int> field_to_index;
        auto current_rule_set = rules;
        int num_fields = static_cast<int>(all_tickets.front().size());
        auto field_set = rv::iota(0, num_fields) | r::to<std::unordered_set<int>>();

        while (!current_rule_set.empty()) {
            auto [field, index] = find_one_field(current_rule_set, tickets, field_set);
            field_to_index[field] = index;
            current_rule_set = current_rule_set |
                rv::filter([field](auto&& rule) {return rule.field != field; }) |
                r::to<std::vector<rule>>();
            field_set.erase(index);
        }

        return field_to_index;
    }

    uint64_t do_part_2(const std::vector<rule>& rules, const ticket& my_ticket,
            const std::vector<ticket>& all_tickets) {
        auto field_to_index = find_field_to_index_mapping(rules, all_tickets);
        return r::fold_left(
            field_to_index | rv::filter(
                    [](auto&& pair) {
                        return pair.first.contains("departure");
                    }
                ) | rv::values | rv::transform(
                    [&my_ticket](auto index) {
                        return my_ticket[index];
                    }
                ),
            1,
            std::multiplies<uint64_t>()
        );
    }
}

void aoc::y2020::day_16(const std::string& title) {
    auto [rules, my_ticket, tickets] = parse(
        aoc::file_to_string_vector(aoc::input_path(2020, 16))
    );
    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            tickets | rv::transform( 
                [&rules](const ticket& ticket)->int{
                    auto invalid = not_valid_value(ticket, rules);
                    return (invalid) ? *invalid : 0;
                }
            ),
            0,
            std::plus<>()
        )
    ); 
    std::println("  part 2: {}", do_part_2(rules, my_ticket, tickets));
}