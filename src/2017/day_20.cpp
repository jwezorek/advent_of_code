
#include "../util/util.h"
#include "../util/vec3.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using vec = aoc::vec3<int64_t>;

    struct particle {
        vec pos;
        vec vel;
        vec acc;
    };

    particle parse_particle(const std::string& str) {
        auto v = aoc::extract_numbers_int64(str, true);
        return {
            {v[0],v[1],v[2]},
            {v[3],v[4],v[5]},
            {v[6],v[7],v[8]}
        };
    }

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    bool no_acc_or_speeding_up(int64_t vel, int64_t acc) {
        if (acc == 0) {
            return true;
        }
        return sgn(acc) == sgn(vel);
    }

    bool is_speeding_up(const particle& p) {
        return no_acc_or_speeding_up(p.vel.x, p.acc.x) &&
            no_acc_or_speeding_up(p.vel.y, p.acc.y) &&
            no_acc_or_speeding_up(p.vel.z, p.acc.z);
    }

    int64_t distance_from_origin(const particle& p) {
        return std::abs(p.pos.x) + std::abs(p.pos.y) + std::abs(p.pos.z);
    }

    bool is_moving_away_from_origin(const particle& old_p, const particle& new_p) {
        return distance_from_origin(new_p) > distance_from_origin(old_p);
    }

    void update_particle(particle& p) {
        p.vel = p.vel + p.acc;
        p.pos = p.pos + p.vel;
    }

    bool all_are_speeding_up(const std::vector<particle>& particles) {
        return r::find_if(
                particles,
                [](auto&& p)->bool {
                    return !is_speeding_up(p);
                }
            ) == particles.end();
    }

    bool all_are_moving_away(
            const std::vector<particle>& old_p, const std::vector<particle>& new_p) {
        auto zipped = rv::zip(old_p, new_p);
        return r::find_if(
            zipped,
            [](auto&& pair) {
                const auto& [old, new_] = pair;
                return !is_moving_away_from_origin(old, new_);
            }
        ) == zipped.end();
    }

    int closest_to_origin(const std::vector<particle>& particles) {
        auto iter = r::min_element(
            particles,
            [](auto&& lhs, auto&& rhs) {
                return distance_from_origin(lhs) < distance_from_origin(rhs);
            }
        );
        return iter - particles.begin();
    }

    int do_part_1(const std::vector<particle>& inp) {
        auto prev = inp;
        auto current = inp;

        int steady_state_count = 0;
        while (steady_state_count < 500) {

            bool all_moving_away = all_are_moving_away(prev, current);
            bool all_speeding_up = all_are_speeding_up(current);
            if (all_moving_away && all_speeding_up) {
                ++steady_state_count;
            }

            prev = current;
            for (auto& p : current) {
                update_particle(p);
            }
        }

        return closest_to_origin(current);
    }
}

void aoc::y2017::day_20(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 20)
        ) | rv::transform(
            parse_particle
        ) | r::to<std::vector>();

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", 0);
    
}
