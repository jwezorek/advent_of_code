
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <stack>
#include <string_view>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using strings = std::vector<std::string>;

    std::tuple<strings, strings> parse_inp(const strings& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        return {
            aoc::extract_alphabetic(groups[0][0]),
            std::move(groups.back())
        };
    }

    auto next_states(const strings& towels, size_t i, const std::string& design) {
        return towels |
            rv::filter(
                [&design,i](const auto& towel)->bool {
                    if (towel.size() > design.size() - i) {
                        return false;
                    }
                    std::string_view sv(design.data() + i, towel.size());
                    return sv == towel;
                }
            ) | rv::transform(
                [=](auto&& towel)->size_t {
                    return i + towel.size();
                }
            );
    }

    bool is_valid_design(const strings& towels, const std::string& design) {
        std::unordered_set<size_t> visited;
        std::stack<size_t> stack;
        stack.push( 0 );

        while (!stack.empty()) {
            auto curr_state = stack.top();
            stack.pop();

            if (curr_state == design.size()) {
                return true;
            }

            if (visited.contains(curr_state)) {
                continue;
            }
            visited.insert(curr_state);

            for (auto next : next_states(towels, curr_state, design)) {
                stack.push(next);
            }
        }

        return false;
    }

    using count_map = std::unordered_map<size_t, uint64_t>;

    uint64_t count_all_arrangements(
            count_map& count, const strings& towels, size_t i, const std::string& design) {

        if (i == design.size()) {
            return 1;
        }

        if (count.contains(i)) {
            return count[i];
        }

        uint64_t sum = 0;
        for (auto j : next_states(towels, i, design)) {
            sum += count_all_arrangements(count, towels, j, design);
        }

        count[i] = sum;
        return sum;
    }

    uint64_t count_all_arrangements(const strings& words, const std::string& str) {
        std::unordered_map<size_t, uint64_t> count;
        return count_all_arrangements(count, words, 0, str);
    }
}

void aoc::y2024::day_19(const std::string& title) {

    auto [towels, designs] = parse_inp(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 19)
        )
    );

    std::println("--- Day 19: {} ---", title);

    std::println("  part 1: {}",
        r::count_if(
            designs,
            [&](const std::string& design) {
                return is_valid_design(towels, design);
            }
        )
    );

    std::println("  part 2: {}",
        r::fold_left(
            designs | rv::transform(
                [&](auto&& design) {
                    return count_all_arrangements(towels, design);
                }
            ),
            0,
            std::plus<>()
        )
    );
    
}
