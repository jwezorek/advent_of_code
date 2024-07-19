#include "../util/util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <numeric>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct axis_state {
        int position;
        int velocity;

        axis_state(int p = 0, int v = 0) : position{ p }, velocity{ v } {
        }
    };

    constexpr int k_x_axis = 0;
    constexpr int k_y_axis = 1;
    constexpr int k_z_axis = 2;

    using system_axis_state = std::array<axis_state, 4>;
    using system_state = std::array<system_axis_state, 3>;

    struct system_axis_state_hash {
        size_t operator()(const system_axis_state& sys_axis) const {
            size_t seed = 0;
            for (const auto& moon_axis : sys_axis) {
                boost::hash_combine( seed, moon_axis.position );
                boost::hash_combine( seed, moon_axis.velocity );
            }
            return seed;
        }
    };

    bool operator==(const system_axis_state& lhs, const system_axis_state& rhs) {
        for (const auto& [a,b] : rv::zip(lhs,rhs)) {
            if (a.position != b.position || a.velocity != b.velocity) {
                return false;
            }
        }
        return true;
    }

    system_state parse_system_state(const std::vector<std::string>& inp) {
        auto moon_positions = inp | rv::transform(
            [](auto&& line)->std::tuple<int,int,int> {
                auto vals = aoc::extract_numbers(line, true);
                return { vals[0], vals[1], vals[2] };
            }
        ) | r::to<std::vector>();

        system_state sys = {{
            {{ {}, {}, {}, {} }},
            {{ {}, {}, {}, {} }},
            {{ {}, {}, {}, {} }}
        }};

        for (const auto& [moon_index, position] : rv::enumerate(moon_positions)) {
            auto [x, y, z] = position;
            sys[k_x_axis][moon_index].position = x;
            sys[k_y_axis][moon_index].position = y;
            sys[k_z_axis][moon_index].position = z;
        }

        return sys;
    }

    int vel_delta_on_axis(const axis_state& lhs, const axis_state& rhs) {
        if (lhs.position == rhs.position) {
            return 0;
        }
        return (rhs.position > lhs.position) ? 1 : -1;
    }

    void update_vel_on_axis(system_axis_state& state) {
        for (auto [i, j] : aoc::two_combos_indices(state.size())) {
            auto i_to_j_delta = vel_delta_on_axis(state[i], state[j]);
            state[i].velocity += i_to_j_delta;
            state[j].velocity -= i_to_j_delta;
        }
    }

    void simulate_n_timesteps_on_axis(system_axis_state& sys_axis_state, int n) {
        for (int i = 0; i < n; ++i) {
            update_vel_on_axis(sys_axis_state);
            for (auto& moon_axis_state : sys_axis_state) {
                moon_axis_state.position += moon_axis_state.velocity;
            }
        }
    }

    std::array<int, 6> moon_state(system_state& sys_state, int moon_index) {
        return {
            sys_state[k_x_axis][moon_index].position,
            sys_state[k_y_axis][moon_index].position,
            sys_state[k_z_axis][moon_index].position,

            sys_state[k_x_axis][moon_index].velocity,
            sys_state[k_y_axis][moon_index].velocity,
            sys_state[k_z_axis][moon_index].velocity
        };
    }

    int energy_after_n_timesteps(system_state& sys_state, int n) {
        auto sys = sys_state;

        simulate_n_timesteps_on_axis(sys[k_x_axis], n);
        simulate_n_timesteps_on_axis(sys[k_y_axis], n);
        simulate_n_timesteps_on_axis(sys[k_z_axis], n);

        return r::fold_left(
            rv::iota(0,4) | rv::transform(
                [&](auto moon_index)->int {
                    auto [x, y, z, x_vel, y_vel, z_vel] = moon_state(sys, moon_index);
                    return (std::abs(x) + std::abs(y) + std::abs(z)) *
                        (std::abs(x_vel) + std::abs(y_vel) + std::abs(z_vel));
                }
            ),
            0,
            std::plus<>()
        );
    }

    // this function assumes that the intial state of each axis eventually repeats,
    // and is the first state to repeat...

    int64_t find_cycle_on_axis(const system_axis_state& initial_state) {
        int64_t steps = 0;
        auto state = initial_state;

        do {
            simulate_n_timesteps_on_axis(state, 1);
            ++steps;
        } while (state != initial_state);

        return steps;
    }
 }

void aoc::y2019::day_12(const std::string& title) {

    auto initial_sys_state = parse_system_state(
            aoc::file_to_string_vector(aoc::input_path(2019, 12))
        );

    std::println("--- Day 12: {} ---", title);

    std::println("  part 1: {}",
        energy_after_n_timesteps(initial_sys_state, 1000)
    );

    std::println("  part 2: {}",
        std::lcm( 
            find_cycle_on_axis(initial_sys_state[0]),
            std::lcm(
                find_cycle_on_axis(initial_sys_state[1]),
                find_cycle_on_axis(initial_sys_state[2])
            )
        )
    );
}