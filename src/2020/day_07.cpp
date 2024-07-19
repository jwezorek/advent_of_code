#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct edge {
        std::string bag;
        int num;
    };

    using graph = std::unordered_map<std::string, std::vector<edge>>;
    struct rule {
        std::string bag;
        std::vector<edge> containees;
    };

    edge parse_containee(const std::string& str) {
        auto containee = aoc::trim(str);
        auto parts = aoc::split(containee, ' ');
        return {
            parts[1] + " " + parts[2],
            std::stoi(parts[0])
        };
    }

    std::vector<edge> parse_containees(const std::string& str) {
        if (str.contains("no other")) {
            return {};
        }
        return aoc::split(str, ',') | rv::transform(
            parse_containee
        ) | r::to<std::vector<edge>>();
    }

    rule parse_rule(const std::string& str) {
        auto inp = std::regex_replace(str, std::regex("contain"), "|");
        inp = std::regex_replace(inp, std::regex("bags"), "");
        inp = std::regex_replace(inp, std::regex("bag"), "");
        inp = aoc::trim(std::regex_replace(inp, std::regex("\\."), ""));
        auto parts = aoc::split(inp, '|');
        return {
            aoc::trim(parts.front()),
            parse_containees(parts.back())
        };
    }

    graph build_graph(const std::vector<rule>& rules) {
        return rules |
            rv::transform(
                [](auto&& rule)->graph::value_type {
                    const auto& [node, edges] = rule;
                    return { node, std::move(edges) };
                }
            ) | r::to<graph>();
    }

    graph invert_graph(const graph& g) {
        graph inverted;
        for (const auto& [bag, edges] : g) {
            for (const auto& e : edges) {
                inverted[e.bag].emplace_back(bag, e.num);
            }
        }
        return inverted;
    }

    int do_part1(const graph& g) {
        auto graph = invert_graph(g);
        std::unordered_set<std::string> visited;
        std::queue<std::string> queue;
        queue.push("shiny gold");
        while (!queue.empty()) {
            auto curr_bag = queue.front();
            queue.pop();
            if (visited.contains(curr_bag)) {
                continue;
            }
            visited.insert(curr_bag);
            if (graph.contains(curr_bag)) {
                for (const auto& neighbor : graph.at(curr_bag)) {
                    queue.push(neighbor.bag);
                }
            }
        }
        return static_cast<int>(visited.size()) - 1;
    }

    int64_t count_bags_aux(std::unordered_map<std::string, int64_t>& memoize, 
            const graph& g, const std::string& bag) {
        if (memoize.contains(bag)) {
            return memoize.at(bag);
        }
        int64_t sum = 1;
        for (const auto& containee : g.at(bag)) {
            sum += containee.num * count_bags_aux(memoize, g, containee.bag);
        }
        memoize[bag] = sum;
        return sum;
    }

    int64_t count_contained_bags(const graph& g, const std::string& bag) {
        std::unordered_map<std::string, int64_t> memoize;
        return count_bags_aux(memoize, g, bag) - 1;
    }
}

void aoc::y2020::day_07(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 7));
    auto rules = input | rv::transform(parse_rule) | r::to<std::vector<rule>>();
    auto graph = build_graph(rules);

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", do_part1(graph));
    std::println("  part 2: {}", count_contained_bags(graph, "shiny gold"));
}