#include "y2023.h"
#include "../util/util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_set>
#include <tuple>

#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct loc {
        int col;
        int row;
    };

    loc operator+(const loc& lhs, const loc& rhs) {
        return {
            lhs.col + rhs.col,
            lhs.row + rhs.row
        };
    }

    bool operator==(const loc& lhs, const loc& rhs) {
        return lhs.col == rhs.col && lhs.row == rhs.row;
    }

    struct loc_hash {
        size_t operator()(const loc& loc) const {
            size_t seed = 0;
            boost::hash_combine(seed, loc.col);
            boost::hash_combine(seed, loc.row);
            return seed;
        }
    };

    using loc_set = std::unordered_set<loc, loc_hash>;
    using grid = std::vector<std::string>;

    enum direction {
        north = 0,
        west,
        south,
        east
    };

    struct beam {
        loc pos;
        direction dir;
    };

    bool operator==(const beam& lhs, const beam& rhs) {
        return lhs.pos == rhs.pos && lhs.dir == rhs.dir;
    }

    struct beam_hasher {
        size_t operator()(const beam& beam) const {
            size_t seed = 0;
            boost::hash_combine(seed, beam.pos.col);
            boost::hash_combine(seed, beam.pos.row);
            boost::hash_combine(seed, beam.dir);
            return seed;
        }
    };

    using beam_set = std::unordered_set<beam, beam_hasher>;

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    loc direction_to_offset(direction dir) {
        static const std::array<loc, 4> offsets = {{
            {0,-1}, // north
            {-1,0}, // west
            {0, 1}, // south
            {1, 0}  // east
        }};
        return offsets[dir];
    }

    std::optional<loc> next_loc(const grid& grid, const beam& beam) {
        auto [cols, rows] = dimensions(grid);

        auto new_pos = beam.pos + direction_to_offset(beam.dir);
        if (new_pos.col < 0 || new_pos.row < 0 ||
                new_pos.col >= cols || new_pos.row >= rows) {
            return {};
        }

        return new_pos;
    }

    std::vector<beam> simulate_splitter(const loc& loc, direction dir, bool is_horz) {
        static const std::array< std::array<std::vector<direction>, 2>, 4> splitters = { {
            {{ {west,east}, {north} }}, // north
            {{ {west}, {north,south} }}, // west
            {{  {west,east} , {south} }}, // south
            {{ {east} , {north,south} }}  // east
        } };
        int splitter_orientation = is_horz ? 0 : 1;
        const auto& new_directions = splitters[dir][splitter_orientation];
        return new_directions |
            rv::transform(
                [&](direction new_dir)->beam {
                    return { loc, new_dir };
                }
            ) | r::to< std::vector<beam>>();
    }

    beam simulate_mirror(const loc& loc, direction dir, bool is_sw_to_ne_mirror) {
        static const std::array< std::array<direction, 2>, 4> mirrors = { {
            {{east, west}}, // north
            {{south, north}}, // west
            {{west, east}}, // south
            {{north, south}}  // east
        }};
        int mirror_orientation = is_sw_to_ne_mirror ? 0 : 1;

        return { loc, mirrors[dir][mirror_orientation]};
    }

    std::vector<beam> simulate_beam(const grid& grid, const beam& beam) {
        auto loc = next_loc(grid, beam);
        if (!loc) {
            return {};
        }
        auto tile = grid[loc->row][loc->col];
        if (tile == '.') {
            return { {*loc, beam.dir} };
        }
        if (tile == '|' || tile == '-') {
            return simulate_splitter(*loc, beam.dir, tile == '-');
        }
        return { simulate_mirror(*loc, beam.dir, tile == '/') };
    }

    std::vector<beam> simulate_beams(const grid& grid, const std::vector<beam>& beams) {
        std::vector<beam> output;
        for (const auto& beam : beams) {
            auto moved = simulate_beam(grid, beam);
            r::copy(moved, std::back_inserter(output));
        }
        return output;
    }

    int simulate_contraption(const grid& g, const beam& start) {
        std::vector<beam> beams = { start };
        beam_set visited;

        while (!beams.empty()) {
            beams = simulate_beams(g, beams) | rv::filter(
                    [&visited](auto&& b) {return !visited.contains(b); }
                ) | r::to<std::vector<beam>>();
            r::copy(beams, std::inserter(visited, visited.begin()));
        }

        auto energized = visited | rv::transform([](auto&& b) {return b.pos; }) | r::to<loc_set>();
        return energized.size();
    }

    std::vector<beam> edge_starts(const grid& g) {
        auto [cols, rows] = dimensions(g);
        std::vector<beam> beams;
        for (int col = 0; col < cols; ++col) {
            beams.emplace_back( loc{ col,-1 }, south);
            beams.emplace_back( loc{ col, rows }, north);
        }
        for (int row = 0; row < rows; ++row) {
            beams.emplace_back(loc{ -1, row }, east);
            beams.emplace_back(loc{ cols, row}, west);
        }
        return beams;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_16(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 16));

    std::println("--- Day 16: {0} ---\n", title);

    std::println("  part 1: {}", 
        simulate_contraption(input, { {-1,0}, east })
    );

    std::println("  part 2: {}",
        r::max( 
            edge_starts(input) |  rv::transform(
                [&](auto&& start)->int {
                    return simulate_contraption(input, start);
                }
            )
        )
    );
    
}