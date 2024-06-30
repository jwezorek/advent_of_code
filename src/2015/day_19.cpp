#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct rule {
        std::string from;
        std::string to;
    };

    rule parse_rule(const std::string& str) {
        auto words = aoc::extract_alphabetic(str);
        return { words[0], words[1] };
    }

    std::tuple<std::vector<rule>, std::string> parse_input(const std::vector<std::string>& inp) {
        auto pieces = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            pieces[0] | rv::transform(
                    parse_rule
                ) | r::to<std::vector>(),
            pieces[1].front()
        };
    }

    std::string apply_rule_at(const rule& rule, const std::string& str, int pos) {
        auto front = std::string_view(str).substr(0, pos);
        auto end = pos + rule.from.size();
        auto back = std::string_view(str).substr(end, str.size() - end);
        std::stringstream ss;
        ss << front << rule.to << back;
        return ss.str();
    }

    std::vector<int> applicable_sites(const rule& rule, const std::string& str) {
        return rv::enumerate(
                str | rv::slide(rule.from.size())
            ) | rv::transform(
                [&](auto&& v)->int {
                    auto [i, rng] = v;
                    auto site = rng | r::to<std::string>();
                    return (site == rule.from) ? static_cast<int>(i) : -1;
                }
            ) | rv::filter(
                [](int i) {
                    return i >= 0;
                }
            ) | r::to<std::vector>();
    }

    int number_of_unique_rule_applications(const std::vector<rule>& rules, const std::string& seed) {
        std::unordered_set<std::string> results;
        for (const auto& rule : rules) {
            auto sites = applicable_sites(rule, seed);
            for (auto site : sites) {
                auto result = apply_rule_at(rule, seed, site);
                results.insert(result);
            }
        }
        return results.size();
    }

    std::vector<std::string> apply_rules(
            const std::vector<rule>& rules, const std::string& seed) {
        std::vector<std::string> out;
        for (const auto& rule : rules) {
            for (auto site : applicable_sites(rule, seed)) {
                out.push_back(apply_rule_at(rule, seed, site));
            }
        }
        return out;
    }

    std::vector<rule> rules_that_apply_at_site(
            const std::vector<rule>& rules, int site, const std::string& seed) {
        return rules | rv::filter(
                [&](auto&& rule) {
                    if (seed.size() - site < rule.from.size()) {
                        return false;
                    }
                    auto str_at_site = std::string_view(seed).substr(site, rule.from.size());
                    return str_at_site == rule.from;
                }
            ) | r::to<std::vector>();
    }

    struct reduce_state {
        std::string mol;
        int steps;
    };

    int reduce_molecule_steps(const std::vector<rule>& rule_set, const std::string& mol) {
        auto rules = rule_set | rv::transform(
                [](auto&& ru)->rule {
                    return { ru.to, ru.from };
                }
            ) | r::to<std::vector>();

        r::sort(rules,
            [](auto&& lhs, auto&& rhs) {
                return lhs.from.size() > rhs.from.size();
            }
        );
        std::unordered_set<std::string> visited;
        std::queue< reduce_state> queue;
        queue.push({ mol, 0 });

        while (! queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current.mol)) {
                continue;
            }
            visited.insert(current.mol);

            if (current.mol == "e") {
                return current.steps;
            }

            // push the two rule applications that reduce the molecule the most.
            // (straight greedy does not work with my input)
            std::vector<reduce_state> next;
            for (auto&& rule : rules) {
                auto sites = applicable_sites(rule, current.mol);
                for (int i : sites) {
                    auto next_molecule = apply_rule_at(rule, current.mol, i);
                    next.push_back({ next_molecule, current.steps + 1 });
                    if (next.size() == 2) {
                        break;
                    }
                }
                if (next.size() == 2) {
                    break;
                }
            }

            for (auto s : next | rv::take(2)) {
                queue.push(s);
            }
        }
        return -1;
    }
}

void aoc::y2015::day_19(const std::string& title) {

    auto [rules, molecule] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2015, 19))
    );

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}",
        number_of_unique_rule_applications(rules, molecule)
    );

    std::println("  part 2: {}", 
        reduce_molecule_steps(rules, molecule)
    );
}