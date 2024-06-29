#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include "../json.hpp"

namespace r = std::ranges;
namespace rv = std::ranges::views;

using json = nlohmann::json;

/*------------------------------------------------------------------------------------------------*/

namespace {

    int recursive_sum_of_numbers_of_non_red_objects(const json& json) {

        if (json.is_number()) {
            auto num = json.get<int>();
            return num;
        }

        if (json.is_array() || json.is_object()) {
            int sum = 0;
            if (json.is_object()) {
                for (auto item : json.items()) {
                    if (item.value().is_string() && item.value().get<std::string>() == "red") {
                        return 0;
                    }
                }
            }
            for (auto item : json.items()) {
                sum += recursive_sum_of_numbers_of_non_red_objects(item.value());
            }
            return sum;
        }

        return 0;
    }

}

void aoc::y2015::day_12(const std::string& title) {

    auto inp = aoc::file_to_string(aoc::input_path(2015, 12));

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left(
            aoc::extract_numbers(inp, true),
            0,
            std::plus<>()
        )
    );
    
    std::println("  part 2: {}",
        recursive_sum_of_numbers_of_non_red_objects(
            json::parse(inp)
        )
    );
}