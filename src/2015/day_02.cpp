#include "../util/util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using box_dimensions = std::array<int, 3>;
    box_dimensions string_to_dimenions(const std::string& str) {
        auto dims = aoc::extract_numbers(str);
        return { dims[0] , dims[1] , dims[2] };
    }

    int area_of_paper_needed(const box_dimensions& b) {
        int l = b[0];
        int w = b[1];
        int h = b[2];

        int min_side = std::min({ l * w, w * h, l * h });
        int area = 2*l*w + 2*w*h + 2*h*l;

        return area + min_side;
    }

    int length_of_ribbon(const box_dimensions& b) {
        int l = b[0];
        int w = b[1];
        int h = b[2];

        auto ribbon_fn = [](int d1, int d2) {
                return 2 * d1 + 2 * d2;
            };
        auto bow = l * w * h;
        int ribbon = std::min({ ribbon_fn(l, w), ribbon_fn(w,h), ribbon_fn(l,h) });

        return ribbon + bow;
    }
}

void aoc::y2015::day_02(const std::string& title) {

    auto boxes = aoc::file_to_string_vector(
            aoc::input_path(2015, 2)
        ) | rv::transform(
            string_to_dimenions
        ) | r::to<std::vector>();

    std::println("--- Day 2: {} ---", title);
    std::println("  part 1: {}",
        r::fold_left( boxes | rv::transform(area_of_paper_needed), 0, std::plus<>() )
    );
    std::println("  part 2: {}",
        r::fold_left( boxes | rv::transform(length_of_ribbon), 0, std::plus<>() )
    );
}