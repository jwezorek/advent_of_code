
#include "../util/util.h"
#include "y2025.h"
#include <queue>
#include <functional>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct machine {
        std::string lights;
        std::vector<std::vector<int>> wiring;
        std::vector<int> joltage;
    };

    std::tuple<std::string, std::string, std::string> chunk_input(const std::string& str) {
        auto pieces = aoc::split(str, ']');
        auto pieces2 = aoc::split(pieces[1], '{');
        return { pieces[0], pieces2[0], pieces2[1] };
    }

    machine parse_machine(const std::string& inp) {
        auto [lights_str, wire_str, jolt_str] = chunk_input(inp);
        return {
            lights_str | rv::drop(1) | r::to<std::string>(),

            aoc::split(wire_str, ')') | rv::transform(
                [](auto&& s) {return aoc::extract_numbers(s); }
            ) | rv::filter(
                [](auto&& v) {return !v.empty(); } 
            ) | r::to<std::vector>(),

            aoc::extract_numbers(jolt_str)
        };
    }

    uint32_t lights_str_to_num(const std::string& str) {
        auto s = str;
        r::replace(s, '.', '0');
        r::replace(s, '#', '1');
        return static_cast<uint32_t>(std::stoul(s, nullptr, 2));
    }

    uint32_t wiring_schematics_to_num(const std::vector<int>& vals, size_t n) {
        uint32_t output = 0;
        for (auto v : vals) {
            auto shift = static_cast<int>(n) - v - 1;
            uint32_t mask = 1 << shift;
            output = output | mask;
        }
        return output;
    }

    struct state {
        uint32_t val;
        int count;
    };

    int fewest_button_presses(const machine& m) {
        auto target = lights_str_to_num(m.lights);
        auto schematics = m.wiring | rv::transform(
                [&](auto&& vals) {
                    return wiring_schematics_to_num(vals, m.lights.size());
                }
            ) | r::to<std::vector>();

        std::unordered_set<uint32_t> visited;
        std::queue<state> queue;
        queue.push({ 0, 0 });

        while (!queue.empty()) {
            auto curr = queue.front();
            queue.pop();

            if (curr.val == target) {
                return curr.count;
            }

            if (visited.contains(curr.val)) {
                continue;
            }
            visited.insert(curr.val);

            for (const auto btn_press : schematics) {
                queue.push({
                    curr.val ^ btn_press,
                    curr.count + 1
                });
            }
            
        }

        return -1;
    }

    int fewest_total_button_presses(const std::vector<machine>& inp) {
        return r::fold_left(
            inp | rv::transform(fewest_button_presses),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2025::day_10(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 10)
        ) | rv::transform(
            parse_machine
        ) | r::to<std::vector>();

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", fewest_total_button_presses(inp) );
    std::println("  part 2: {}", 0);
    
}
