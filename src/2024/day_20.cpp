
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

    struct state {
        point loc;
        bool used_cheat;

        bool operator==(const state& s) const {
            return loc == s.loc && used_cheat == s.used_cheat;
        }
    };

    struct hash_state {
        size_t operator()(const state& s) const {
            size_t seed = aoc::hash_vec2<int>{}(s.loc);
            boost::hash_combine(seed, s.used_cheat);
            return seed;
        }
    };

    bool in_bounds(const point& loc, int wd, int hgt) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < wd && loc.y < hgt;
    }

    std::vector<cheat> find_cheats(const racetrack& rt) {
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

    std::vector<state> next_states(const state& s, const racetrack& rt, std::optional<cheat> cheat) {

        if (s.used_cheat && cheat) {
            if (s.loc == cheat->cheat1) {
                return  std::vector<state>{ {cheat->cheat2, true} };
            }
        }

        const static std::array<point, 4> deltas = {{
            {0,-1},{1,0},{0,1},{-1,0}
        }};

        auto next_states = deltas | rv::transform(
            [&](auto&& delta)->state {
                return { delta + s.loc, s.used_cheat };
            }
        ) | rv::filter(
            [&](auto&& new_state) {
                return !rt.walls.contains(new_state.loc);
            }
        ) | r::to<std::vector>();

        if (!s.used_cheat && cheat) {
            for (auto delta : deltas) {
                auto adj = s.loc + delta;
                if (adj == cheat->cheat1) {
                    next_states.emplace_back(
                        adj,
                        true
                    );
                }
            }
        }

        return next_states;
    }

    using state_set = std::unordered_set<state, hash_state>;

    int shortest_path(const racetrack& rt, std::optional<cheat> cheat) {
        std::queue<std::tuple<state, int>> queue;
        state_set visited;

        queue.emplace(state{ rt.start,false }, 0);
        while (!queue.empty()) {
            auto [state, dist] = queue.front();
            queue.pop();

            if (state.loc == rt.end) {
                return dist;
            }

            if (visited.contains(state)) {
                continue;
            }
            visited.insert(state);

            for (auto next_state : next_states(state, rt, cheat)) {
                queue.emplace(next_state, dist+1);
            }
        }

        return -1;
    }

    int do_part_1(const racetrack& rt) {
        auto no_cheats = shortest_path(rt, {});
        auto cheats = find_cheats(rt);
        return std::count_if(
            std::execution::par,
            cheats.begin(),
            cheats.end(),
            [&](auto&& cheat)->bool {
                auto with_cheat = shortest_path(rt, cheat);
                return (no_cheats - with_cheat >= 100);
            }
        );
    }

    int64_t do_part_2(const racetrack& rt) {
        auto empty_locs = rv::cartesian_product(rv::iota(0, rt.wd), rv::iota(0, rt.hgt)) |
            rv::transform(
                [&](auto&& tup)->point {
                    const auto& [x, y] = tup;
                    return { x,y };
                }
            ) | rv::filter(
                [&](auto&& pt) {
                    return !rt.walls.contains(pt);
                }
            ) | r::to<std::vector>();

        auto internal_walls = rt.walls | rv::filter(
                [&](auto&& wall) {
                    return wall.x != 0 && wall.y != 0 && wall.x != rt.wd - 1 && wall.y != rt.hgt - 1;
                }
            ) | r::to<std::vector>();

        auto cheats = rv::cartesian_product(internal_walls, empty_locs) | rv::filter(
                [](auto&& pair) {
                    const auto& [from, to] = pair;
                    return std::abs(from.x - to.x) + std::abs(from.y - to.y) + 1 <= 20;
                }
            ) | rv::transform(
                [](auto&& pair)->cheat {
                    const auto& [from, to] = pair;
                    return { from,to };
                }
            ) | r::to<std::vector>();

        auto no_cheats = shortest_path(rt, {});

        return std::count_if(
            std::execution::par,
            cheats.begin(),
            cheats.end(),
            [&](auto&& cheat)->bool {
                auto with_cheat = shortest_path(rt, cheat);
                return (no_cheats - with_cheat >= 100);
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
    std::println("  part 1: {}", do_part_1(inp) );
    //std::println("  part 2: {}", do_part_2(inp) );
    
}
