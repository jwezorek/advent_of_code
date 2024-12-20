
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
        point_set empty;
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
                continue;
            } else if (tile == 'S') {
                rt.start = { x,y };
            } else if (tile == 'E') {
                rt.end = { x,y };
            }
            rt.empty.insert(point{ x,y });
        }

        return rt;
    }

    struct cheat {
        point entrance;
        point exit;
    };

    bool in_bounds(const point& loc, int wd, int hgt) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < wd && loc.y < hgt;
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

        auto from_start_len = from_start.at(cheat.entrance);
        auto cheat_len = manhattan_distance(cheat.entrance, cheat.exit);
        auto to_end_len = to_end.at(cheat.exit);

        return  from_start_len + cheat_len + to_end_len;
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
                    if (pt == point{ 0, 0 }) {
                        return false;
                    }
                    return manhattan_distance({ 0,0 }, pt) <= sz;
                }
            ) | r::to<std::vector>();
    }

    std::vector<cheat> find_cheats(const racetrack& rt, int max_dist) {
        auto neighborhood = manhattan_dist_neighborhood(max_dist);

        return rv::cartesian_product(
            rt.empty, neighborhood
        ) | rv::transform(
            [](auto&& pair)->cheat {
                const auto& [start_loc, delta] = pair;
                return { start_loc, start_loc + delta };
            }
        ) | rv::filter(
            [&](auto&& cheat) {
                return in_bounds(cheat.exit, rt.wd, rt.hgt) &&
                    rt.empty.contains(cheat.exit);
            }
        ) | r::to<std::vector>();
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
