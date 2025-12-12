
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;

    using shape = std::vector<point>;
    using shapes = std::vector<shape>;

    struct region {
        int wd;
        int hgt;
        std::vector<int> shape_counts;
    };
    using regions = std::vector<region>;

    shape to_shape(const std::vector<std::string>& inp) {
        auto grid = inp | rv::drop(1) | r::to<std::vector>();
        shape s;
        for (int y = 0; y < 3; ++y) {
            for (int x = 0; x < 3; ++x) {
                if (grid[y][x] == '#') {
                    s.push_back(point{ x,y } - point{ 1,1 });
                }
            }
        }
        return s;
    }

    shapes parse_shapes(std::span<const std::string> inp) {
        auto pieces = aoc::group_strings_separated_by_blank_lines(inp | r::to<std::vector>());
        return pieces | rv::transform(to_shape) | r::to<std::vector>();
    }

    regions parse_regions(std::span<const std::string> inp) {
        return inp | rv::drop(1) | rv::transform(
            [](auto&& str)->region {
                auto vals = aoc::extract_numbers(str);
                return {
                    .wd = vals[0],
                    .hgt = vals[1],
                    .shape_counts = vals | rv::drop(2) | r::to<std::vector>()
                };
            }
        ) | r::to<std::vector>();
    }

    std::tuple<shapes, regions> parse_input(const std::vector<std::string>& inp) {
        auto i = std::prev(r::find_if(inp, [](auto&& str) {return str.contains('x'); }));
        return {
            parse_shapes({inp.cbegin(), i}),
            parse_regions({i, inp.cend()})
        };
    }

    bool is_packing_feasible(const region& rgn, const shapes& shapes) {
        int area = rgn.wd * rgn.hgt;
        int shape_area = r::fold_left(
            rv::zip(
                shapes, rgn.shape_counts) | rv::transform(
                    [](auto&& pair)->int {
                        const auto& [s, count] = pair;
                        return s.size() * count;
                    }
                ),
            0,
            std::plus<>()
        );
        return area >= shape_area;
    }
}

void aoc::y2025::day_12(const std::string& title) {

    auto [shapes, regions] = parse_input(
            aoc::file_to_string_vector(
                aoc::input_path(2025, 12)
            )
        );

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", 
        r::count_if(
            regions,
            [&](auto&& rgn) {
                return is_packing_feasible(rgn, shapes);
            }
        )
    );
    std::println("  part 2: {}", "<xmas freebie>");
    
}
