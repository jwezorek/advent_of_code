
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int find_quotient(const std::vector<int>& vals) {
        for (auto [u, v] : aoc::two_combinations(vals)) {
            if (v < u) {
                std::swap(u, v);
            }
            if (v % u == 0) {
                return v / u;
            }
        }
        throw std::runtime_error("bad input");
    }

}

void aoc::y2017::day_02(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 2)
        ) | rv::transform(
            [](const auto& str) {
                return extract_numbers(str, true);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);

    std::println("  part 1: {}", 
        r::fold_left(
            inp | rv::transform(
                [](const std::vector<int>& row) { return r::max(row) - r::min(row); }
            ),
            0,
            std::plus<>()
        )
    );

    std::println("  part 2: {}",
        r::fold_left(
            inp | rv::transform( find_quotient ),
            0,
            std::plus<>()
        )
    );
    
}
