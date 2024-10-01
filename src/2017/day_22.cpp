
#include "../util/util.h"
#include "../util/vec2.h"
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
    using point = aoc::vec2<int>;

    enum virus_state {
        clean,
        weakened,
        infected,
        flagged
    };

    using virus_map = aoc::vec2_map<int, virus_state>;

    virus_map get_infected_cells(const std::vector<std::string>& grid) {
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid.front().size());
        virus_map map;
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                point pt(x, y);
                if (grid[pt.y][pt.x] == '#') {
                    map[pt] = infected;
                }
            }
        }
        return map;
    }

    point initial_loc(const std::vector<std::string>& grid) {
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid.front().size());
        return { wd / 2, hgt / 2 };
    }

    point direction_delta(int dir) {
        const static std::array<point, 4> directions =
            {{ {0,-1} , {1,0}, {0,1}, {-1,0} }};
        return directions[dir];
    }

    int turn_right(int dir) {
        return (dir + 1) % 4;
    }

    int turn_left(int dir) {
        return (dir + 3) % 4;
    }

    int reverse(int dir) {
        return (dir + 2) % 4;
    }

    struct infector_state {
        point pos;
        int dir;
    };

    using update_func = std::function<bool(virus_map&, infector_state&)>;

    bool simple_update(virus_map& infected_map, infector_state& state) {
        bool new_infection = false;

        if (infected_map.contains(state.pos)) {
            state.dir = turn_right(state.dir);
        } else {
            state.dir = turn_left(state.dir);
        }

        if (!infected_map.contains(state.pos)) {
            infected_map[state.pos] = infected;
            new_infection = true;
        } else {
            infected_map.erase(state.pos);
        }

        state.pos = state.pos + direction_delta(state.dir);
        return new_infection;
    }

    bool multistate_update(virus_map& infected_map, infector_state& state) {
        bool new_infection = false;

        virus_state virus = (infected_map.contains(state.pos)) ?
            infected_map.at(state.pos) : clean;
        if (virus == clean) {
            state.dir = turn_left(state.dir);
        } else if (virus == infected) {
            state.dir = turn_right(state.dir);
        } else if (virus == flagged) {
            state.dir = reverse(state.dir);
        }

        if (virus == clean) {
            infected_map[state.pos] = weakened;
        } else if (virus == weakened) {
            infected_map[state.pos] = infected;
            new_infection = true;
        } else if (virus == infected) {
            infected_map[state.pos] = flagged;
        } else if (virus == flagged) {
            infected_map.erase(state.pos);
        }

        state.pos = state.pos + direction_delta(state.dir);
        return new_infection;
    }

    int simulate_virus(
            const update_func& update, const virus_map& inp, const point& start, int n) {

        auto infected = inp;
        infector_state state{ start, 0 };
        int count = 0;

        for (int i = 0; i < n; ++i) {
            if (update(infected, state)) {
                ++count;
            }
        }

        return count;
    }
}

void aoc::y2017::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 22)
        ); 

    auto starting_loc = initial_loc(inp);
    auto infected_set = get_infected_cells(inp);

    std::println("--- Day 22: {} ---", title);

    std::println("  part 1: {}", 
        simulate_virus(
            simple_update, infected_set, starting_loc, 10000
        )
    );

    std::println("  part 2: {}",
        simulate_virus(
            multistate_update, infected_set, starting_loc, 10000000
        )
    );

}
