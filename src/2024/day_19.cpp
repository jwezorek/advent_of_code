
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

    auto next_states(const strings& words, size_t i, const std::string& str) {
        return words |
            rv::filter(
                [&str,i](const auto& word)->bool {
                    if (word.size() > str.size() - i) {
                        return false;
                    }
                    auto beg = str.begin();
                    std::string_view sv(&*(str.begin() + i), word.size());
                    return sv == word;
                }
            ) | rv::transform(
                [=](auto&& word)->size_t {
                    return i + word.size();
                }
            );
    }

    bool test_design(const strings& words, const std::string& str) {
        std::unordered_set<size_t> visited;
        std::stack<size_t> stack;
        stack.push( 0 );

        while (!stack.empty()) {
            auto curr_state = stack.top();
            stack.pop();

            if (curr_state == str.size()) {
                return true;
            }

            if (visited.contains(curr_state)) {
                continue;
            }
            visited.insert(curr_state);
            for (auto next : next_states(words, curr_state, str)) {
                stack.push(next);
            }
        }

        return false;
    }

    uint64_t count_all_paths(std::unordered_map<size_t, uint64_t>& count, const strings& words, size_t i, const std::string& str) {
        if (i == str.size()) {
            return 1;
        }

        uint64_t sum = 0;
        for (auto j : next_states(words, i, str)) {
            if (count.contains(j)) {
                sum += count[j];
                continue;
            }
            sum += count_all_paths(count, words, j, str);
        }

        count[i] = sum;
        return sum;
    }

    uint64_t count_all_paths(const strings& words, const std::string& str) {
        std::unordered_map<size_t, uint64_t> count;
        return count_all_paths(count, words, 0, str);
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
                return test_design(towels, design);
            }
        )
    );

    std::println("  part 2: {}",
        r::fold_left(
            designs | rv::transform(
                [&](auto&& design) {
                    return count_all_paths(towels, design);
                }
            ),
            0,
            std::plus<>()
        )
    );
    
}
