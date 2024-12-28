
#include "../util/util.h"
#include "../util/vec2.h"
#include "../2015/md5.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using point = aoc::vec2<int>;

    point dir_to_delta(char dir) {
        static const std::unordered_map<char, point> tbl = {
            {'U', {0,-1}},
            {'R', {1,0}},
            {'D', {0,1}},
            {'L', {-1,0}}
        };
        return tbl.at(dir);
    }

    struct bounds {
        int wd;
        int hgt;
    };

    bool in_bounds(const point& loc, const bounds& bnds) {
        return loc.x >= 0 && loc.y >= 0 && loc.x < bnds.wd && loc.y < bnds.hgt;
    }

    struct vault {
        std::string passcode;
        point src;
        point dst;
        bounds bounds;
    };

    struct state {
        point loc;
        std::string path;
    };

    bool is_open_char(char ch) {
        return ch >= 'b' && ch <= 'f';
    }

    std::vector<state> next_states(const state& s, const vault& vault) {
        static const std::array<char, 4> directions = { 'U','D','L','R' };
        auto open_directions = rv::enumerate(
            md5(vault.passcode + s.path) | rv::take(4)
        ) | rv::transform(
            [&](auto&& pair)->char {
                const auto& [index, hash_code] = pair;
                return (is_open_char(hash_code)) ? directions[index] : ' ';
            }
        ) | rv::filter(
            [](char ch) {return ch != ' '; }
        ) | r::to<std::string>();

        return open_directions | rv::transform(
                [&](char dir)->state {
                    return { s.loc + dir_to_delta(dir), s.path + dir };
                }
            ) | rv::filter(
                [&](auto&& new_state) {
                    return in_bounds(new_state.loc, vault.bounds);
                }
            ) | r::to<std::vector>();
    }

    std::string shortest_path(const vault& vault) {
        std::queue<state> queue;
        queue.emplace(vault.src, "");
        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();

            if (curr_state.loc == vault.dst) {
                return curr_state.path;
            }

            for (const auto& next_state : next_states(curr_state, vault)) {
                queue.push(next_state);
            }
        }

        return {};
    }

    size_t longest_path(const vault& vault) {
        std::queue<state> queue;
        size_t longest_path = 0;
        queue.emplace(vault.src, "");
        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();

            if (curr_state.loc == vault.dst) {
                longest_path = std::max(longest_path, curr_state.path.size());
                continue;
            }

            for (const auto& next_state : next_states(curr_state, vault)) {
                queue.push(next_state);
            }
        }

        return longest_path;
    }
}

void aoc::y2016::day_17(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2016, 17)
        ); 

    vault v{
        inp,
        {0,0},
        {3,3},
        {4,4}
    };

    std::println("--- Day 17: {} ---", title);
    std::println("  part 1: {}", shortest_path(v) );
    std::println("  part 2: {}", longest_path(v) );
    
}
