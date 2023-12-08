#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <tuple>
#include <algorithm>
#include <unordered_map>
#include <utility>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    using node_map = std::unordered_map<std::string, std::pair<std::string, std::string>>;
    std::tuple<std::string, node_map> parse_input(const std::vector<std::string>& lines) {
        return {
            lines.front(),
            lines | rv::drop(2) |
                rv::transform(
                    [](const std::string& line)->node_map::value_type {
                        auto parts = aoc::extract_alphabetic(line);
                        return { parts[0], std::pair{parts[1],parts[2]}  };
                    }
                ) | r::to<node_map>()
        };
    }

    int64_t find_cycle(const std::string& instr, const node_map& nodes, const std::string& node) {
        int64_t cycle = 0;
        std::string curr_node = node;
        size_t iter = 0;
        while (curr_node.back() != 'Z') {
            curr_node = (instr.at(iter) == 'L') ? nodes.at(curr_node).first : nodes.at(curr_node).second;
            ++cycle;
            ++iter;
            iter = (iter == instr.size()) ? 0 : iter;
        }
        if (iter != 0) {
            throw std::runtime_error("something is wrong");
        }
        return cycle;
    }

    int do_part1(const std::string& instructions, const node_map& nodes) {
        int count = 0;
        std::string curr_node = "AAA";
        std::string::const_iterator iter = instructions.begin();
        while (curr_node != "ZZZ") {
            count++;
            const auto& next = nodes.at(curr_node);
            curr_node = (*iter == 'L') ? next.first : next.second;

            ++iter;
            if (iter == instructions.end()) {
                iter = instructions.begin();
            }
        }
        return count;
    }

    std::vector<std::string> next_state(
            const std::vector<std::string>& curr_state, const node_map& nodes, bool left) {
        return curr_state |
            rv::transform(
                [&](const std::string& node) {
                    return (left) ? nodes.at(node).first : nodes.at(node).second;
                }
            ) | r::to<std::vector<std::string>>();
    }

    int64_t do_part2(const std::string& instructions, const node_map& nodes) {
        std::vector<std::string> initial_state = nodes |
            rv::transform(
                [](const auto& pair) {
                    return pair.first;
                }
            ) | rv::filter(
                [](const std::string& node) {return node.back() == 'A'; }
            ) | r::to<std::vector<std::string>>();

        auto cycles = initial_state |
                rv::transform(
                    [&](const std::string& start)->int64_t {
                        return find_cycle(instructions, nodes, start);
                    }
            ) | r::to<std::vector<int64_t>>();

            return r::fold_left(
                cycles,
                1,
                [](int64_t lhs, int64_t rhs)->int64_t {
                    return std::lcm(lhs, rhs);
                }
            );
    }

}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_08(const std::string& title) {

    const auto& [instructions, nodes] = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2023, 8))
    );

    std::println("--- Day 8: {0} ---\n", title);
    std::println("  part 1: {}", do_part1(instructions, nodes));
    std::println("  part 2: {}", do_part2(instructions, nodes));

}