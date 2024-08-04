#include "../util/util.h"
#include "../util/vec2.h"
#include "../util/grid.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <set>
#include <boost/functional/hash.hpp>

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

    enum tool {
        neither = 0,
        torch,
        climbing_gear
    };

    auto other_tools(tool t) {
        return rv::iota(0, 3) | rv::transform(
            [](auto i)->tool {
                return static_cast<tool>(i);
            }
        ) | rv::filter(
            [t](auto other) {
                return other != t;
            }
        );
    }

    struct traversal_state {
        point loc;
        tool tool_equipped;

        bool operator==(const traversal_state& ts) const {
            return loc == ts.loc && tool_equipped == ts.tool_equipped;
        }
    };

    struct traversal_state_hash {
        size_t operator()(const traversal_state& ts) const {
            size_t seed = 0;
            boost::hash_combine(seed, ts.loc.x);
            boost::hash_combine(seed, ts.loc.y);
            boost::hash_combine(seed, ts.tool_equipped);
            return seed;
        }
    };

    const std::set<tool>& tools_allowed_in_cave(cave_type ct) {
        static const std::unordered_map<cave_type, std::set<tool>> tbl = {
            {rocky, {climbing_gear, torch} },
            {wet, {climbing_gear, neither}},
            {narrow, {torch, neither}}
        };
        return tbl.at(ct);
    }

    auto adjacent_locs(const point& loc, const grid<cave_type>& cavern) {
        auto [wd, hgt] = cavern.dimensions();
        const static std::array<point, 4> deltas = {{
            {0,-1},{1,0},{0,1},{-1,0}
        }};
        return deltas | rv::transform(
            [loc](const auto& delta) {
                return loc + delta;
            }
        ) | rv::filter(
            [wd, hgt](auto&& pt) {
                return pt.x >= 0 && pt.y >= 0 && pt.x < wd && pt.y < hgt;
            }
        );
    }

    std::vector<std::tuple<traversal_state,int>> next_moves(const grid<cave_type>& cavern, const traversal_state& ts) {
        std::vector<std::tuple<traversal_state, int>> moves;
        for (auto&& next_loc : adjacent_locs(ts.loc, cavern)) {
            const auto& tools_allowed_there = tools_allowed_in_cave(cavern(next_loc));
            if (tools_allowed_there.contains(ts.tool_equipped)) {
                moves.emplace_back(
                    traversal_state{ next_loc, ts.tool_equipped },
                    1
                );
            }
        }
        const auto& tools_allowed_here = tools_allowed_in_cave(cavern(ts.loc));
        for (auto new_tool : other_tools(ts.tool_equipped)) {
            if (tools_allowed_here.contains(new_tool)) {
                moves.emplace_back(
                    traversal_state{ ts.loc, new_tool },
                    7
                );
            }
        }
        return moves;
    }

    using time_map = std::unordered_map<traversal_state, int, traversal_state_hash>;

    int lowest_time(const time_map& map, const point& target) {
        return r::min(
            map | rv::filter(
                [target](auto&& pair) {
                    const auto& [state, time] = pair;
                    return state.loc == target && state.tool_equipped == torch;
                }
            ) | rv::values
        );
    }

    int lowest_time_path(const grid<cave_type>& cavern, const point& target) {

        time_map elapsed_time;

        aoc::priority_queue<traversal_state, traversal_state_hash> queue;
        traversal_state start_state = { {0, 0}, torch };
        elapsed_time[start_state] = 0;
        queue.insert(start_state, 0);

        while (!queue.empty()) {
            auto u = queue.extract_min();
            auto moves = next_moves(cavern, u);

            for (auto [v, time_to_v] : moves) {
                auto dist_to_u = elapsed_time.at(u);
                auto dist_through_u_to_v = dist_to_u + time_to_v;
                auto curr_dist_to_v = elapsed_time.contains(v) ?
                    elapsed_time.at(v) :
                    std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    elapsed_time[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return lowest_time(elapsed_time, target);
    }

    int do_part_2(const scan_results& scan) {
        const int k_padding = 250;
        auto cave_system = build_cave_system(
            scan, scan.target.x + k_padding, 
            scan.target.y + k_padding
        );
        return lowest_time_path(cave_system, scan.target);
    }
}

void aoc::y2018::day_22(const std::string& title) {

    auto scan = parse(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 22)
        )
    );

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", do_part_1(scan) );
    std::println("  part 2: {}", do_part_2(scan) );
    
}
