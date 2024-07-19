#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <queue>

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

    enum direction {
        north = 1,
        south = 2,
        west = 3,
        east = 4
    };

    auto directions() {
        return rv::iota(1, 5) | rv::transform(
            [](auto n)->direction {
                return static_cast<direction>(n);
            }
        );
    }

    point dir_to_delta(direction dir) {
        static const std::unordered_map< direction, point> tbl = {
            {north, {0,-1}},
            {south, {0,1}},
            {west, {-1,0}},
            {east, {1,0}}
        };
        return tbl.at(dir);
    }

    enum move_result {
        blocked = 0,
        moved = 1,
        moved_to_goal = 2
    };

    using computer = aoc::intcode_computer;

    struct state {
        computer icc;
        point loc;
        int dist;
        bool is_on_goal;
    };

    move_result run_robot(computer& robot, direction dir) {
        move_result result;
        robot.run_until_event(static_cast<int64_t>(dir));
        robot.run_until_event();
        return static_cast<move_result>(robot.output());
    }

    std::vector<state> adjacent_states(const state& state) {
        std::vector<::state> output;
        for (auto dir : directions()) {
            auto robot = state.icc;
            auto result = run_robot(robot, dir);
            if (result == moved || result == moved_to_goal) {
                output.emplace_back(
                    robot,
                    state.loc + dir_to_delta(dir),
                    state.dist + 1,
                    result == moved_to_goal
                );
            }
        }
        return output;
    }

    struct traversal_info {
        int shortest_path;
        point_set map;
        point goal_location;
    };

    traversal_info perform_traversal(const std::vector<int64_t>& program) {

        traversal_info ti;

        std::queue<state> queue;
        queue.push({ computer{program}, {0,0}, 0, false });

        while (!queue.empty()) {
            auto state = queue.front();
            queue.pop();

            if (ti.map.contains(state.loc)) {
                continue;
            }
            ti.map.insert(state.loc);

            if (state.is_on_goal) {
                ti.goal_location = state.loc;
                ti.shortest_path = state.dist;
            }

            for (const auto& adj : adjacent_states(state)) {
                queue.push(adj);
            }
         
        }
        return ti;
    }

    point_set new_frontier(const point_set& old_frontier, const point_set& open_locations) {
        point_set new_frontier;
        for (auto pt : old_frontier) {
            for (auto delta : directions() | rv::transform(dir_to_delta)) {
                auto new_pt = pt + delta;
                if (open_locations.contains(new_pt)) {
                    new_frontier.insert(new_pt);
                }
            }
        }
        return new_frontier;
    }

    int flood_fill_duration(const point& seed, const point_set& map) {
        auto open_locations = map;
        point_set frontier;
        frontier.insert(seed);
        int time_step = 0;
        while (!open_locations.empty()) {
            for (auto pt : frontier) {
                open_locations.erase(pt);
            }
            frontier = new_frontier(frontier, open_locations);
            ++time_step;
        }
        return time_step - 1;
    }
}

void aoc::y2019::day_15(const std::string& title) {

    auto program = split(
            aoc::file_to_string(aoc::input_path(2019, 15)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    auto trav_info = perform_traversal(program);

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", trav_info.shortest_path);
    std::println("  part 2: {}",
        flood_fill_duration(trav_info.goal_location, trav_info.map)
    );
}