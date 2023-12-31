#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum room : unsigned char {
        leftwing_2 = 0, leftwing_1, a_vestibule, a_b_hallway, b_vestibule, b_c_hallway, c_vestibule,  
        c_d_hallway, d_vestibule, rightwing_1, rightwing_2, side_room_a_1, side_room_a_2, 
        side_room_b_1, side_room_b_2, side_room_c_1, side_room_c_2, side_room_d_1, side_room_d_2
    };

    room vestibule_for_amphi(char amphi) {
        static const std::unordered_map<char, room> tbl = {
            {'A', a_vestibule},
            {'B', b_vestibule},
            {'C', c_vestibule},
            {'D', d_vestibule}
        };
        return tbl.at(amphi);
    }

    room side_room_for_amphi(char amphi, int i) {
        static const std::array<std::unordered_map<char, room>, 2> tbl = {{{
            {'A', side_room_a_1},
            {'B', side_room_b_1},
            {'C', side_room_c_1},
            {'D', side_room_d_1}
        }, {
            {'A', side_room_a_2},
            {'B', side_room_b_2},
            {'C', side_room_c_2},
            {'D', side_room_d_2}
        }}};
        return tbl.at(i).at(amphi);
    }

    auto hall_rooms() {
        return rv::iota(0, static_cast<int>(side_room_a_1)) |
            rv::transform(
                [](int i) {
                    return static_cast<room>(i);
                }
        );
    }

    using borrow_graph = std::unordered_map<room, std::vector<room>>;
    borrow_graph make_borrow_graph() {
        borrow_graph graph;
        for (auto [left, right] : hall_rooms() | rv::pairwise) {
            graph[left].push_back(right);
            graph[right].push_back(left);
        }
        for (char amphi = 'A'; amphi <= 'D'; ++amphi) {
            auto vestibule = vestibule_for_amphi(amphi);
            auto side_room_1 = side_room_for_amphi(amphi, 0);
            auto side_room_2 = side_room_for_amphi(amphi, 1);

            graph[vestibule].push_back(side_room_1);
            graph[side_room_1].push_back(vestibule);
            graph[side_room_1].push_back(side_room_2);
            graph[side_room_2].push_back(side_room_1);
        }
    }

    using state = std::array<room, 8>;

    bool operator==(const state& lhs, const state& rhs) {
        for (auto [left, right] : rv::zip(lhs, rhs)) {
            if (left != right) {
                return false;
            }
        }
        return true;
    }

    struct state_hash {
        size_t operator()(const state& state) const {
            size_t seed = 0;
            for (auto room : state) {
                boost::hash_combine(seed, room);
            }
            return seed;
        }
    };

    std::vector<state> neighboring_states(const state& state) {
        return {};
    }

    state initial_state(const std::vector<std::string>& inp) {
        return {};
    }

}

void aoc::y2021::day_23(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 23));

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}