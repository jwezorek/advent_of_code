#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using key_t = std::tuple<int, int>;
    struct tuple_hash_t {
        std::size_t operator()(const key_t& k) const {
            return std::hash<int>{}(std::get<0>(k) ^ std::get<1>(k));
        }
    };

    using grid_loc_t = std::tuple<int, int>;
    using loc_set_t = std::unordered_set<grid_loc_t, tuple_hash_t>;
    using grid_t = std::vector<std::vector<int>>;

    grid_t strings_to_grid(const std::vector<std::string>& lines) {
        std::vector<std::vector<int>> rows(lines.size());
        std::transform(lines.begin(), lines.end(), rows.begin(),
            [](const std::string& line) {
                std::vector<int> cells(line.size());
                std::transform(line.begin(), line.end(), cells.begin(),
                    [](char numeral)->int {return numeral - '0'; }
                );
                return cells;
            }
        );
        return rows;
    }

    std::vector<grid_loc_t> increase_energies(grid_t& grid) {
        auto cols = grid[0].size();
        std::vector<grid_loc_t> initial_flashes;
        for (int row = 0; row < grid.size(); ++row) {
            for (int col = 0; col < cols; ++col) {
                if (++(grid[row][col]) > 9) {
                    initial_flashes.push_back({ row,col });
                }
            }
        }
        return initial_flashes;
    }

    constexpr std::array<std::array<int, 2>, 8> get_neighbor_offsets() {
        std::array<std::array<int, 2>, 8> locs = {};
        int i = 0;
        for (int row = -1; row <= 1; ++row) {
            for (int col = -1; col <= 1; ++col) {
                if (row != 0 || col != 0) {
                    locs[i++] = std::array<int, 2>{ row, col };
                }
            }
        }
        return locs;
    }

    std::vector<grid_loc_t> get_neighbor_locs(const grid_t& grid, int row, int col) {
        constexpr auto neighbor_offsets = get_neighbor_offsets();
        int rows = grid.size();
        int cols = grid[0].size();
        std::vector<std::tuple<int, int>> neighbors;
        for (const auto& [dx, dy] : neighbor_offsets) {
            auto r = row + dy;
            auto c = col + dx;
            if (r >= 0 && r < rows && c >= 0 && c < cols) {
                neighbors.emplace_back(r, c);
            }
        }

        return neighbors;
    }

    void do_flash(grid_t& grid, grid_loc_t loc, loc_set_t& flashed, std::vector<grid_loc_t>& unflashed) {
        if (flashed.find(loc) != flashed.end()) {
            return;
        }
        flashed.insert(loc);
        auto [row, col] = loc;
        for (auto& neighbor : get_neighbor_locs(grid, row, col)) {
            auto [r, c] = neighbor;
            if (++grid[r][c] > 9) {
                unflashed.push_back(neighbor);
            }
        }
    }

    loc_set_t handle_flashes(grid_t& grid, const std::vector<grid_loc_t>& intial_flashes) {
        auto unflashed = intial_flashes;
        loc_set_t flashed;

        while (!unflashed.empty()) {
            auto flach_loc = unflashed.back();
            unflashed.pop_back();
            do_flash(grid, flach_loc, flashed, unflashed);
        }

        return flashed;
    }

    void zero_out_flash_sites(grid_t& grid, const loc_set_t& flash_sites) {
        for (const auto& [row, col] : flash_sites) {
            grid[row][col] = 0;
        }
    }

    int perform_one_step(grid_t& grid) {
        auto initial_flashes = increase_energies(grid);
        auto flash_site_set = handle_flashes(grid, initial_flashes);
        zero_out_flash_sites(grid, flash_site_set);
        return flash_site_set.size();
    }
    /*
    void display_grid(const grid_t& grid) {
        for (const auto& row : grid) {
            for (int v : row) {
                std::cout << v;
            }
            std::cout << "\n";
        }
    }
    */
    int simulate(grid_t& grid, int n) {
        int sum = 0;
        for (int i = 0; i < n; ++i) {
            sum += perform_one_step(grid);
        }
        return sum;
    }

    int simulate_until_synchonization(grid_t& grid) {
        int num_flashes = 0;
        int step = 0;
        int num_cells = grid.size() * grid[0].size();
        do {
            num_flashes = perform_one_step(grid);
            step++;
        } while (num_flashes != num_cells);
        return step;
    }
}

void aoc::y2021::day_11(const std::string& title) {
    auto part_1_cells = aoc::strings_to_2D_array_of_digits(
        aoc::file_to_string_vector(aoc::input_path(2021, 11))
    );
    auto part_2_cells = part_1_cells;

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", simulate(part_1_cells, 100));
    std::println("  part 2: {}", simulate_until_synchonization(part_2_cells));
}