#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    enum class color {
        red, green, blue
    };

    struct cube_item {
        color col;
        int count;
    };

    using cube_set = std::vector<cube_item>;

    struct game_info {
        int id;
        std::vector<cube_set> sets;
    };

    color get_color(const std::string& str) {
        if (str.contains('d')) {
            return color::red;
        }
        if (str.contains('g')) {
            return color::green;
        }
        if (str.contains('b')) {
            return color::blue;
        }
        throw std::runtime_error("parsing color");
    }

    cube_item parse_cube_item(const std::string& str) {
        return {
            get_color(str),
            std::stoi(aoc::remove_nonnumeric(str))
        };
    }

    cube_set parse_set(const std::string& inp) {
        return aoc::split(inp, ',') |
            rv::transform(parse_cube_item) |
            r::to<std::vector< cube_item>>();
    }

    std::vector<cube_set> parse_sets(const std::string& inp) {
        auto str = aoc::trim(inp);
        return aoc::split(str, ';') |
            rv::transform(parse_set) |
            r::to<std::vector<cube_set>>();
    }

    game_info parse_line(const std::string& str) {
        auto game_pair = aoc::split(str, ':');
        game_info output;
        output.id = std::stoi(aoc::remove_nonnumeric(game_pair.front()));
        output.sets = parse_sets(game_pair.back());
        return output;
    }

    int get_color(color col, const cube_item& ci) {
        return ci.col == col ? ci.count : 0;
    }

    int max_color(color col, const cube_set& cs) {
        return r::max(cs | rv::transform([col](const auto& ci) {return get_color(col, ci); }));
    }

    bool is_possible(const game_info& gi) {
        int red = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::red, cs); }));
        int green = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::green, cs); }));
        int blue = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::blue, cs); }));

        return red <= 12 && green <= 13 && blue <= 14;
    }

    int do_part_1(const std::vector<game_info> games) {
        int sum = 0;
        for (const auto& gi : games) {
            if (is_possible(gi)) {
                sum += gi.id;
            }
        }
        return sum;
    }

    int power(const game_info& gi) {
        int red = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::red, cs); }));
        int green = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::green, cs); }));
        int blue = r::max(gi.sets | rv::transform([](const auto& cs) {return max_color(color::blue, cs); }));
        return red * blue * green;
    }

    int do_part_2(const std::vector<game_info> games) {
        int sum = 0;
        for (const auto& gi : games) {
            sum += power(gi);
        }
        return sum;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_02(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 2)) |
        rv::transform(parse_line) |
        r::to<std::vector<game_info>>();

    std::println("--- Day 2: {0} ---\n", title);
    
    std::println("  part 1: {0}", do_part_1(input));
    std::println("  part 2: {0}", do_part_2(input));
}