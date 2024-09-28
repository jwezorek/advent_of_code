
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <iterator>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using grid = std::vector<std::string>;
    using loc = aoc::vec2<int>;

    loc find_start(const grid& g) {
        auto iter = r::find(g.front(), '|');
        return { static_cast<int>(std::distance(g.front().begin(), iter)), 0 };
    }

    bool in_bounds(const grid& g, const loc& p) {
        return p.x >= 0 && p.y >= 0 && p.x < g.front().size() && p.y < g.size();
    }

    auto neighbors(loc p) {
        const static std::array<loc, 4> ary = { {
            {1,0}, {0,1}, {-1,0}, {0,-1}
        } };
        return ary | rv::transform(
            [p](auto&& adj) {
                return p + adj;
            }
        );
    }

    std::tuple<std::string,int> traverse_tubes(const grid& g) {

        auto loc = find_start(g);
        auto prev = loc;
        auto dir = ::loc{ 0,1 };
        std::stringstream letters;
        int steps = 0;
        char tile = 0;

        while (in_bounds(g, loc) && ((tile = g[loc.y][loc.x]) != ' ')) {
            if (std::isalpha(tile)) {
                letters << tile;
            } else if (tile == '+') {
                auto adj = neighbors(loc);
                auto next = r::find_if(adj,
                    [&](auto&& p)->bool {
                        if (!in_bounds(g, p) || p == prev) {
                            return false;
                        }
                        return g[p.y][p.x] != ' ';
                    }
                );
                dir = *next - loc;
            }
            prev = loc;
            loc = loc + dir;
            ++steps;
        }

        return { letters.str(), steps };

    }

}

void aoc::y2017::day_19(const std::string& title) {

    auto [letters, steps] = traverse_tubes(
        aoc::file_to_string_vector(
            aoc::input_path(2017, 19)
        )
    ); 

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", letters );
    std::println("  part 2: {}", steps );
    
}
