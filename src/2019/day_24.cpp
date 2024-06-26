#include "../util.h"
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
    struct vec3 {
        int x;
        int y;
        int z;

        bool operator==(const vec3& v) const {
            return x == v.x && y == v.y && z == v.z;
        }
    };

    vec3 operator+(const vec3& lhs, const vec3& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    struct vec3_hash {
        size_t operator()(const vec3& v) const {
            size_t seed = 0;
            boost::hash_combine(seed, v.x);
            boost::hash_combine(seed, v.y);
            boost::hash_combine(seed, v.z);
            return seed;
        }
    };

    using vec3_set = std::unordered_set<vec3, vec3_hash>;

    constexpr int k_dim = 5;

    auto neighbor_deltas() {
        static const std::array<vec3, 4> deltas = { { {0,-1,0}, {1,0,0}, {0,1,0}, {-1,0,0} } };
        return rv::all(deltas);
    }

    template<typename T>
    using five_by_five_array = std::array<std::array<T, k_dim>, k_dim>;

    five_by_five_array<std::vector<vec3>> trans_level_neighbors() {
        return {{
            {{ {{2,1,-1}, {1,2,-1}},{{2,1,-1}},{{2,1,-1}},{{2,1,-1}},{{2,1,-1},{3,2,-1}} }},
            {{ {{1,2,-1}}, {}, {{0,0,1},{1,0,1},{2,0,1},{3,0,1},{4,0,1}}, {}, {{3,2,-1}} }},
            {{ {{1,2,-1}}, {{0,0,1},{0,1,1},{0,2,1},{0,3,1},{0,4,1}},{},{{4,0,1},{4,1,1},{4,2,1},{4,3,1},{ 4,4,1 }},{{3,2,-1}}}},
            {{ {{1,2,-1}}, {}, {{0,4,1},{1,4,1},{2,4,1},{3,4,1},{4,4,1}}, {}, {{3,2,-1}} }},
            {{ {{1,2,-1},{2,3,-1}}, {{2,3,-1}}, {{2,3,-1}}, {{2,3,-1}}, {{2,3,-1}, {3,2,-1}} }}
        }};
    }

    vec3_set neighboring_cells(const vec3& v, bool recursive) {
        if (!recursive) {
            return neighbor_deltas() | rv::transform(
                [&v](auto&& delta) {
                    return v + delta;
                }
            ) | rv::filter(
                [](auto&& v) {
                    return v.x >= 0 &&
                        v.y >= 0 &&
                        v.x < k_dim &&
                        v.y < k_dim;
                }
            ) | r::to<vec3_set>();
        }
        auto set = neighboring_cells(v, false);
        set.erase({ 2,2, v.z });

        static const auto trans = trans_level_neighbors();
        for (auto neighbor : trans[v.y][v.x]) {
            set.insert({ neighbor.x, neighbor.y, neighbor.z + v.z });
        }

        return set;
    }

    vec3_set union_sets(const vec3_set& lhs, const vec3_set& rhs) {
        auto union_set = lhs;
        for (const vec3& v : rhs) {
            union_set.insert(v);
        }
        return union_set;
    }

    vec3_set neighboring_cells(const vec3_set& set, bool recursive) {
        vec3_set output_set;
        for (const auto& v : set) {
            output_set = union_sets(output_set, neighboring_cells(v, recursive));
        }
        return output_set;
    }

    int live_neighbors(const vec3_set& bugs, const vec3& loc, bool recursive) {
        return r::count_if(
            neighboring_cells(loc, recursive),
            [&](const vec3& v)->bool {
                return bugs.contains(v);
            }
        );
    }

    vec3_set next_generation(const vec3_set& old_gen, bool recursive) {

        vec3_set next_gen;
        for (auto loc : neighboring_cells(old_gen, recursive)) {
            int neighbors = live_neighbors(old_gen, loc, recursive);
            if (!old_gen.contains(loc)) {
                if (neighbors == 1 || neighbors == 2) {
                    next_gen.insert(loc);
                } 
            } else {
                if (neighbors == 1) {
                    next_gen.insert(loc);
                }
            }
        }

        return next_gen;
    }

    vec3_set grid_to_cell_set(const std::vector<std::string>& g) {
        vec3_set set;
        for (int y = 0; y < k_dim; ++y) {
            for (int x = 0; x < k_dim; ++x) {
                if (g[y][x] == '#') {
                    set.insert({ x,y,0 });
                }
            }
        }
        return set;
    }

    uint64_t memoize(const vec3_set& set) {
        uint64_t memo = 0;
        int binary_digit = 0;
        for (int y = 0; y < k_dim; ++y) {
            for (int x = 0; x < k_dim; ++x) {
                if (set.contains({ x,y })) {
                    memo = (memo | (static_cast<uint64_t>(1) << binary_digit));
                }
                binary_digit++;
            }
        }
        return memo;
    }

    uint64_t do_part_1(const std::vector<std::string>& g) {
        auto bugs = grid_to_cell_set(g);
        std::unordered_set<uint64_t> set;
        uint64_t memo = memoize(bugs);
        while (!set.contains(memo)) {
            set.insert(memo);
            bugs = next_generation(bugs, false);
            memo = memoize(bugs);
        }
        return memo;
    }

    uint64_t do_part_2(const std::vector<std::string>& g) {
        auto bugs = grid_to_cell_set(g);
        for (int i = 0; i < 200; ++i) {
            bugs = next_generation(bugs, true);
        }
        return bugs.size();
    }
}

void aoc::y2019::day_24(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2019, 24));


    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}",
        do_part_1(inp)
    );
    std::println("  part 2: {}",
        do_part_2(inp)
    );
}