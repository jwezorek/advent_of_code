
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <queue>
#include <execution>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_set = aoc::vec2_set<int>;

    template<typename T>
    using point_map = aoc::vec2_map<int, T>;

    struct racetrack {
        point start;
        point end;
        point_set walls;
        int wd;
        int hgt;
    };

    racetrack parse_input(const std::vector<std::string>& inp) {
        racetrack rt;
        rt.wd = static_cast<int>(inp[0].size());
        rt.hgt = static_cast<int>(inp.size());

        for (auto [x, y] : rv::cartesian_product(rv::iota(0, rt.wd), rv::iota(0, rt.hgt))) {
            auto tile = inp[y][x];
            if (tile == '#') {
                rt.walls.insert(point{ x,y });
            } else if (tile == 'S') {
                rt.start = { x,y };
            } else if (tile == 'E') {
                rt.end = { x,y };
            }
        }

        return rt;
    }

    struct cheat {
        point cheat1;
        point cheat2;
    };

    bool in_bounds(const point& loc, int wd, int hgt) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < wd && loc.y < hgt;
    }

    std::vector<cheat> find_simple_cheats(const racetrack& rt) {
        const static std::array<point, 4> deltas = { {
            {0,-1},{1,0},{0,1},{-1,0}
        } };
        std::vector<cheat> cheats;
        for (const auto& [wall,delta] : rv::cartesian_product(rt.walls, deltas)) {
            auto src = (-1 * delta) + wall;
            auto dest = delta + wall;

            if (!in_bounds(src, rt.wd, rt.hgt) || !in_bounds(dest, rt.wd, rt.hgt)) {
                continue;
            }
            
            if (!rt.walls.contains(src) && !rt.walls.contains(dest)) {
                cheats.emplace_back(wall, dest);
            }
        }

        return cheats;
    }

    auto adjacent_locs(const racetrack& rt, const point& loc) {
        const static std::array<point, 4> deltas = { { {0,-1},{1,0},{0,1},{-1,0} } };
        return deltas | rv::transform(
            [loc](auto&& delta) {
                return loc + delta;
            }
        ) | rv::filter(
            [&rt](auto&& new_loc) {
                return !rt.walls.contains(new_loc);
            }
        );
    }

    point_map<int> shortest_path_map( const racetrack& rt, bool start_to_end ) {

        std::queue<std::tuple<point, int>> queue;
        point_map<int> shortest_path;

        point src, dest;
        std::tie(src, dest) = (start_to_end) ?
            std::tuple<point, point>{ rt.start, rt.end} :
            std::tuple<point, point>{ rt.end,rt.start };

        queue.emplace(src, 0);
        while (!queue.empty()) {
            auto [loc, dist] = queue.front();
            queue.pop();

            if (shortest_path.contains(loc)) {
                continue;
            }
            shortest_path[loc] = dist;
            
            for (auto adj : adjacent_locs(rt, loc)) {
                queue.emplace(adj, dist+1);
            }
        }

        return shortest_path;
    }

    int manhattan_distance(const point& u, const point& v) {
        return std::abs(v.x - u.x) + std::abs(v.y - u.y);
    }

    int shortest_path_with_cheat(const point_map<int>& from_start, const point_map<int>& to_end, cheat cheat) {
        const static std::array<point, 4> deltas = { { {0,-1},{1,0},{0,1},{-1,0} } };
        auto entrance_to_cheat = r::min(
            deltas | rv::transform(
                [&](auto&& delta) {
                    return cheat.cheat1 + delta;
                }
            ) | rv::filter(
                [&](auto&& pt) {
                    return from_start.contains(pt);
                }
            ),
            [&](auto&& lhs, auto&& rhs) {
                return from_start.at(lhs) < from_start.at(rhs);
            }
        );
        return from_start.at(entrance_to_cheat) +
            manhattan_distance(cheat.cheat1, cheat.cheat2) +
            to_end.at(cheat.cheat2);
    }

    int64_t count_timesaving_cheats(
            const racetrack& rt, const std::vector<cheat>& cheats, int min_saving) {

        auto from_start = shortest_path_map(rt, true);
        auto to_end = shortest_path_map(rt, false);

        int without_cheating_len = from_start.at(rt.end);

        return r::count_if(
            cheats,
            [&](auto&& cheat) {
                auto len = shortest_path_with_cheat(
                    from_start, to_end, cheat
                );
                return without_cheating_len - len >= min_saving;
            }
        );
    }

    std::vector<point> manhattan_dist_neighborhood(int sz) {
        return rv::cartesian_product(rv::iota(-sz, sz + 1), rv::iota(-sz, sz + 1)) |
            rv::transform(
                [](auto&& tup)->point {
                    auto [x, y] = tup;
                    return { x,y };
                }
            ) | rv::filter(
                [sz](auto&& pt) {
                    if (pt == point{0, 0}) {
                        return false;
                    }
                    return manhattan_distance({ 0,0 }, pt) < sz;
                }
            ) | r::to<std::vector>();
    }

    bool adjacent_to_empty_loc(const racetrack& rt, const point& loc) {
        const static std::array<point, 4> deltas = {{ {0,-1},{1,0},{0,1},{-1,0} }};
        for (const auto& delta : deltas) {
            auto adj = loc + delta;
            if (in_bounds(adj, rt.wd, rt.hgt) && !rt.walls.contains(adj)) {
                return true;
            }
        }
        return false;
    }

    std::vector<cheat> find_cheats(const racetrack& rt, int max_dist) {
        auto cheat_starts = rt.walls | rv::filter(
            [&](auto&& wall) {
                if (wall.x == 0 && wall.y == 0 && wall.x == rt.wd - 1 && wall.y == rt.hgt - 1) {
                    return false;
                }
                return adjacent_to_empty_loc(rt, wall);
            }
        ) | r::to<std::vector>();

        auto neighborhood = manhattan_dist_neighborhood(max_dist);

        return rv::cartesian_product(
            cheat_starts, neighborhood
        ) | rv::transform(
            [](auto&& pair)->cheat {
                const auto& [start_loc, delta] = pair;
                return { start_loc, start_loc + delta };
            }
        ) | rv::filter(
            [&](auto&& cheat) {
                return in_bounds(cheat.cheat2, rt.wd, rt.hgt) && 
                    !rt.walls.contains(cheat.cheat2);
            }
        ) | r::to<std::vector>();
    }
}

void aoc::y2024::day_20(const std::string& title) {

    auto inp = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 20)
        )
    );

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", count_timesaving_cheats(inp, find_cheats(inp, 2), 100) );
    std::println("  part 2: {}", count_timesaving_cheats(inp, find_cheats(inp, 20), 100) );
    
}
