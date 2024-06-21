#include "../util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <format>
#include <fstream>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }
    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    using point_set = std::unordered_set<point, point_hash>;

    using computer = aoc::intcode_computer;

    std::vector<std::string> generate_grid(const std::vector<int64_t>& program) {
        computer rob(program);
        std::stringstream ss;
        rob.run(
            aoc::input_fn{},
            [&ss](int64_t output) {
                ss << static_cast<char>(output);
            }
        );
        auto grid = aoc::split(ss.str(), '\n');
        grid.pop_back(); // teminating EOL makes a blank line at the end...
        return grid;
    }

    char get_tile(int x, int y, const std::vector<std::string>& grid, int wd, int hgt) {
        if (y < 0 || y >= hgt || x < 0 || x >= wd) {
            return '.';
        }
        return grid[y][x];
    }

    std::vector<point> find_intersections(const std::vector<std::string>& grid) {
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());

        std::vector<point> intersections;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (get_tile(x,y, grid, wd, hgt) != '.' &&
                        get_tile(x, y - 1, grid, wd, hgt) != '.' &&
                        get_tile(x, y + 1, grid, wd, hgt) != '.' &&
                        get_tile(x - 1, y, grid, wd, hgt) != '.' &&
                        get_tile(x + 1, y, grid, wd, hgt) != '.') {
                    intersections.emplace_back(x, y);
                }
            }
        }

        return intersections;
    }

    int calculate_alignment_parameters(const std::vector<point>& intersections) {
        return r::fold_left(
            intersections | rv::transform(
                [](auto&& pt)->int {
                    return pt.x * pt.y;
                }
            ),
            0,
            std::plus<>()
        );
    }

    std::string grid_to_svg(const std::vector<std::string>& grid) {
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());
        int marg = 40;
        int dim = 20;

        std::stringstream ss;
        ss << std::format(
            "<svg xmlns = \"http://www.w3.org/2000/svg\" version = \"1.1\" "
            "width = \"{}\" height = \"{}\" >\n", wd * dim + 2*marg, hgt * dim + 2*marg);

        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (grid[y][x] == '.') {
                    continue;
                }
                ss << std::format(
                    "<rect x=\"{}\" y=\"{}\" width=\"{}\" height=\"{}\" fill=\"gray\" stroke=\"black\" />\n",
                    x * dim + marg, y * dim + marg, dim, dim
                );
            }
        }

        ss << "</svg>";

        return ss.str();
    }

    int do_part_2(const std::vector<int64_t>& program) {
        computer comp(program);
        return 0;
    }
}

void aoc::y2019::day_17(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 17)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    auto grid = generate_grid(program);

    std::println("--- Day 17: {} ---", title);

    std::println("  part 1: {}",
        calculate_alignment_parameters(
            find_intersections(grid) 
        )
    );

    std::println("  part 2: {}", do_part_2(program) );
    
}