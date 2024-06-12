#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int required_fuel(int mass, bool fuel_for_fuel) {
        auto fuel_required = mass / 3 - 2;
        fuel_required = (fuel_required >= 0) ? fuel_required : 0;

        if (fuel_for_fuel && fuel_required > 0) {
            fuel_required += required_fuel(fuel_required, true);
        }

        return fuel_required;
    }

}

void aoc::y2019::day_01(const std::string& title) {

    auto nums = aoc::file_to_string_vector(aoc::input_path(2019, 1)) | rv::transform(
        [](auto&& str) {return std::stoi(str); }
    ) | r::to<std::vector<int>>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            nums | rv::transform(
                    [](int mass) {
                        return required_fuel(mass, false); 
                    }
                ),
            0,
            std::plus<>()
        )
    );
    std::println("  part 2: {}",
        r::fold_left(
            nums | rv::transform(
                [](int mass) {
                    return required_fuel(mass, true); 
                }
            ),
            0,
            std::plus<>()
        ));
}