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

        bool operator==(const vec3& p) const {
            return x == p.x && y == p.y && z == p.z;
        }
    };

    vec3 operator+(const vec3& lhs, const vec3& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    struct vec3_hash {
        size_t operator()(const vec3& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            boost::hash_combine(seed, pt.z);
            return seed;
        }
    };

    struct moon {
        vec3 position;
        vec3 velocity;

        moon(const vec3 p) :
            position{ p },
            velocity{ 0, 0, 0 }
        {}
    };

    std::vector<moon> parse_input(const std::vector<std::string>& inp) {
        return inp | rv::transform(
            [](auto&& line)->moon {
                auto vals = aoc::extract_numbers(line, true);
                return moon({ vals[0], vals[1], vals[2] });
            }
        ) | r::to<std::vector>();
    }

    void update_vel_comp(moon& m1, moon& m2, std::function<int&(vec3&)> get) {

        int v1 = get(m1.velocity);
        int v2 = get(m2.velocity);
        int pos1 = get(m1.position);
        int pos2 = get(m2.position);

        if (pos1 == pos2) {
            return;
        }

        auto delta = (pos2 > pos1) ? 1 : -1;
        get(m1.velocity) = v1 + delta;
        get(m2.velocity) = v2 + (-1 * delta);
    }

    void update_velocities(std::vector<moon>& moons) {
        for (auto [i, j] : aoc::two_combos_indices(moons.size())) {
            update_vel_comp( moons[i], moons[j], [](vec3& v)->int& { return v.x; } );    
            update_vel_comp( moons[i], moons[j], [](vec3& v)->int& { return v.y; } );   
            update_vel_comp( moons[i], moons[j], [](vec3& v)->int& { return v.z; } );
        }
    }

    void simulate_moons(std::vector<moon>& moons, int steps) {
        for (int i = 0; i < steps; ++i) {
            update_velocities(moons);
            for (auto& moon : moons) {
                moon.position = moon.position + moon.velocity;
            }
        }
    }

    int energy_of_vec3(const vec3& v) {
        return std::abs(v.x) + std::abs(v.y) + std::abs(v.z);
    }

    int energy_after_n_steps(const std::vector<moon>& inp, int n) {
        auto moons = inp;
        simulate_moons(moons, n);
        return r::fold_left(
            moons | rv::transform(
                [](auto&& m)->int {
                    return energy_of_vec3(m.position) * energy_of_vec3(m.velocity);
                }
            ),
            0,
            std::plus<>()
        );
    }
 }

void aoc::y2019::day_12(const std::string& title) {

    auto moons = parse_input(
        aoc::file_to_string_vector(aoc::input_path(2019, 12))
    );

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}",
        energy_after_n_steps(moons, 1000)
    );
    std::println("  part 2: {}",
        0
    );
}