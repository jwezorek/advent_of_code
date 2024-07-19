#include "../util/util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct instruction {
        char dir;
        int dist;
    };

    using instructions = std::vector<instruction>;

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

    point operator-(const point& lhs, const point& rhs) {
        return  { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    point operator*(int lhs, const point& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    instruction to_instruction(const std::string& inp) {
        return {
            inp.front(),
            std::stoi(
                inp | rv::drop(1) | r::to<std::string>()
            )
        };
    }

    std::vector<instruction> to_instructions(const std::string& str) {
        return aoc::split(str, ',') |
            rv::transform(
                to_instruction
            ) | r::to<std::vector>();
    }

    using loc_to_steps = std::unordered_map<point,int, point_hash>;
    loc_to_steps instructions_to_loc_set(const std::vector<instruction>& instructions) {
        const static std::unordered_map<char, point> dir_to_delta = {
            {'U', {0,1}}, {'L', {-1,0}}, {'D', {0,-1}}, {'R', {1,0}}
        };
        loc_to_steps output;
        point loc = { 0,0 };
        int step_count = 0;
        for (const auto& inst : instructions) {
            for (int i = 0; i < inst.dist; ++i) {
                loc = loc + dir_to_delta.at(inst.dir);
                ++step_count;
                if (!output.contains(loc)) {
                    output[loc] = step_count;
                }
            }
        }
        return output;
    }

    std::vector<point> set_intersections(const loc_to_steps& set_1, const loc_to_steps& set_2) {
        return set_1 |
            rv::filter(
                [&](const auto& kvp)->bool {
                    return set_2.contains(kvp.first);
                }
            ) | rv::keys | r::to<std::vector>();
    }

    int manhattan_dist_from_origin(const point& pt) {
        return std::abs(pt.x) + std::abs(pt.y);
    }

    auto manhattan_distances(const std::vector<point>& points) {
        return points | rv::transform(
            manhattan_dist_from_origin
        );
    }

    auto step_distances(const std::vector<point>& points, 
            const loc_to_steps& map1, const loc_to_steps& map2 ) {
        return points | rv::transform(
            [&](auto&& pt)->int {
                return map1.at(pt) + map2.at(pt);
            }
        );
    }

    int closest_intersection(const std::vector<instruction>& inst_1,
            const std::vector<instruction>& inst_2,
            bool manhattan_distance) {

        auto loc_map_1 = instructions_to_loc_set( inst_1 );
        auto loc_map_2 = instructions_to_loc_set( inst_2 );
        auto intersections = set_intersections( loc_map_1, loc_map_2 );

        if (manhattan_distance) {
            return r::min( manhattan_distances(intersections) );
        } else {
            return r::min(step_distances( intersections, loc_map_1, loc_map_2) );
        }
    }
}

void aoc::y2019::day_03(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2019, 3)
        ) | rv::transform(
            to_instructions
        ) | r::to<std::vector>();

    std::println("--- Day 3: {} ---", title);
    std::println("  part 1: {}",
        closest_intersection(inp.front(), inp.back(), true)
    );
    std::println("  part 2: {}",
        closest_intersection(inp.front(), inp.back(), false)
    );
}