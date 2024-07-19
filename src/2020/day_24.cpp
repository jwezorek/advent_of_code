#include "../util/util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    struct vec2 {
        int x;
        int y;

        bool operator==(vec2 rhs) const {
            return x == rhs.x && y == rhs.y;
        }
    };

    vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    struct hash_vec2 {
        size_t operator()(const vec2& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };
    using vec2_set = std::unordered_set<vec2, hash_vec2>;

    enum direction {
        east, ne, nw, west, sw, se
    };

    auto directions() {
        return rv::iota(0, 6) | rv::transform(
            [](int i) {
                return static_cast<direction>(i);
            }
        );
    }

    std::vector<direction> parse_directions(const std::string& inp) {
        static const std::unordered_map<std::string, direction> directions = {
            {"e",east}, {"ne",ne}, {"nw",nw}, {"w",west}, {"sw",sw}, {"se",se}
        };
        return inp | rv::chunk_by(
            [&](char lhs, char rhs) {
                std::string str(2, ' ');
                str[0] = lhs;
                str[1] = rhs;
                return directions.contains(str);
            }
        ) | rv::transform(
            [&](auto rng)->direction {
                auto str = rng | r::to<std::string>();
                return directions.at(str);
            }
        ) | r::to<std::vector<direction>>();
    }

    vec2 dir_to_delta(direction dir) {
        static const std::unordered_map<direction, vec2> deltas = {
            {east, {1,0}},  {ne, {0,1}},  {nw, {-1,1}}, 
            {west, {-1,0}}, {sw, {0,-1}}, {se, {1,-1}}
        };
        return deltas.at(dir);
    }

    vec2 follow_directions(const std::vector<direction>& dirs) {
        return r::fold_left( dirs, vec2{ 0,0 },
            [](const vec2& lhs, direction rhs)->vec2 {
                return lhs + dir_to_delta(rhs);
            }
        );
    }

    vec2_set flip_tiles(const std::vector<std::vector<direction>>& directions) {
        vec2_set flipped;
        for (auto dirs : directions) {
            auto tile = follow_directions(dirs);
            if (flipped.contains(tile)) {
                flipped.erase(tile);
            } else {
                flipped.insert(tile);
            }
        }
        return flipped;
    }

    auto neighborhood(const vec2& v) {
        return directions() |
            rv::transform(
                [v](direction dir)->vec2 {
                    return v + dir_to_delta(dir);
                }
            );
    }

    vec2_set active_area(const vec2_set& tiles) {
        std::array<vec2_set, 2> sets = {
            tiles | rv::transform(neighborhood) | rv::join | r::to<vec2_set>(),
            tiles
        };
        return sets | rv::join | r::to<vec2_set>();
    }

    int count_neighbors(const vec2& v, const vec2_set& tiles) {
        return r::fold_left(
            neighborhood(v) | rv::transform(
                [&tiles](auto&& neighbor)->int {
                    return (tiles.contains(neighbor)) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    vec2_set do_one_generation(const vec2_set& tiles) {
        vec2_set next_generation;
        auto active_cells = active_area(tiles);
        for (const auto& loc : active_cells) {
            int alive_neighbors = count_neighbors(loc, tiles);
            if (tiles.contains(loc)) { // black cell...
                if (alive_neighbors == 1 || alive_neighbors == 2) {
                    next_generation.insert(loc);
                }
            } else {  // white cell
                if (alive_neighbors == 2) {
                    next_generation.insert(loc);
                }
            }
        }
        return next_generation;
    }

    int do_n_generations(const vec2_set& inp, int n) {
        auto tiles = inp;
        for (int i = 0; i < n; ++i) {
            tiles = do_one_generation(tiles);
        }
        return tiles.size();
    }
}

void aoc::y2020::day_24(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 24)) |
        rv::transform(parse_directions) | r::to<std::vector<std::vector<direction>>>();

    auto initial_tiles = flip_tiles(input);

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", initial_tiles.size());
    std::println("  part 2: {}", do_n_generations(initial_tiles, 100));
}