#include "../util/util.h"
#include "../util/vec2.h"
#include "../util/grid.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum cave_type {
        rocky = 0,
        wet,
        narrow
    };

    template<typename T>
    using grid = aoc::grid<T>;
    using point = aoc::vec2<int>;

    struct scan_results {
        int64_t depth;
        point target;
    };

    scan_results parse(const std::vector<std::string>& inp) {
        auto vals = inp | rv::transform(
                [](auto&& str) {
                    return aoc::extract_numbers(str);
                }
            ) | r::to<std::vector>();
        return {
            vals.front().front(),
            {vals.back().front(), vals.back().back()}
        };
    }

    void calc_geo_index_and_erosion(const scan_results& scan, int wd, int hgt, const point& loc, 
            grid<int64_t>& geo_index, grid<int64_t>& erosion) {

        if ((loc.x == 0 && loc.y == 0) || 
                (loc.x == scan.target.x && loc.y == scan.target.y)) {
            geo_index(loc) = 0;
        }
        else if (loc.y == 0) {
            geo_index(loc) = loc.x * 16807;
        }
        else if (loc.x == 0) {
            geo_index(loc) = loc.y * 48271;
        }
        else {
            geo_index(loc) = erosion(loc + point{ -1,0 }) * erosion(loc + point{ 0,-1 });
        }
        erosion(loc) = (geo_index(loc) + scan.depth) % 20183;

    }

    grid<int64_t> erosion_level(const scan_results& scan, int wd, int hgt) {
        grid<int64_t> geo_index(wd, hgt, 0);
        grid<int64_t> erosion(wd, hgt, 0);

        for (int col = 0; col < wd; ++col) {
            for (int i = col; i < hgt; ++i) {
                calc_geo_index_and_erosion(scan, wd, hgt, { col,i }, geo_index, erosion);
            }
            for (int i = col; i < wd; ++i) {
                calc_geo_index_and_erosion(scan, wd, hgt, { i,col }, geo_index, erosion);
            }
        }

        return erosion;
    }

    grid<cave_type> build_cave_system(const scan_results& scan, int wd, int hgt) {
        auto erosion = erosion_level(scan, wd, hgt);
        return erosion.transform<cave_type>(
            [](const int64_t& ev)->cave_type {
                return static_cast<cave_type>(ev % 3);
            }
        );
    }

    void display(const grid<cave_type>& cave) {
        static const std::array<char, 3> tiles = { {'.','=','|'} };
        auto [wd, hgt] = cave.dimensions();
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                std::print("{}", tiles.at(cave(x, y)));
            }
            std::println("");
        }
    }

    int do_part_1(const scan_results& scan) {
        auto cave_system = build_cave_system(scan, scan.target.x+1, scan.target.y+1);
        return r::fold_left(
            cave_system.values() | rv::transform([](auto ct) {return static_cast<int>(ct); }),
            0,
            std::plus<>()
        );
    }
}

void aoc::y2018::day_22(const std::string& title) {

    auto scan = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 22)
        )
    );

    //scan.depth = 510;
    //scan.target = { 10,10 };

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", do_part_1(scan) );
    std::println("  part 2: {}", 0);
    
}
