
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <tuple>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using rule = std::tuple<int, int>;
    using graph = std::unordered_map<int, std::unordered_set<int>>;
    using update = std::vector<int>;

    graph rules_to_graph(const std::vector<rule>& edges) {
        graph g;
        for (const auto& [u, v] : edges) {
            g[u].insert(v);
        }
        return g;
    }

    std::tuple<graph, std::vector<update>> parse_input(const std::vector<std::string>& inp) {
        auto sections = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            rules_to_graph(
                sections.front() | rv::transform(
                    [](auto&& str)->rule {
                        auto verts = aoc::extract_numbers(str);
                        return { verts[0], verts[1] };
                    }
                ) | r::to<std::vector>()
            ),
            sections.back() | rv::transform(
                [](auto&& str) {
                    return aoc::extract_numbers(str);
                }
            ) | r::to<std::vector>()
        };
    }

    bool is_valid_update(const graph& g, const update& update) {
        for (auto [i, j] : update | rv::pairwise) {
            if (g.at(j).contains(i)) {
                return false;
            }
        }
        return true;
    }

    int do_part_1(const graph& g, const std::vector<update>& updates) {
        return r::fold_left(
            updates | rv::transform(
                [&](auto&& rule) {
                    return is_valid_update(g, rule) ? rule[rule.size() / 2] : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int do_part_2(const graph& g, const std::vector<update>& updates) {
        return r::fold_left(
            updates | rv::transform(
                [&](auto rule) {
                    if (is_valid_update(g, rule)) {
                        return 0;
                    }
                    r::sort(
                        rule,
                        [&](int lhs, int rhs) {
                            return g.at(lhs).contains(rhs);
                        }
                    );
                    return rule[rule.size() / 2];
                }
            ),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2024::day_05(const std::string& title) {

    auto [graph, updates] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 5)
        )
    );

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}", do_part_1(graph, updates));
    std::println("  part 2: {}", do_part_2(graph, updates));
    
}
