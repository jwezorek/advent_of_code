#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    template<typename T>
    std::tuple<int, int> get_wd_and_hgt(const std::vector<std::vector<T>>& ary) {
        return {
            ary[0].size(),
            ary.size()
        };
    }

    template<typename T>
    std::vector<std::tuple<int, int>> get_4cell_neighbor_locs(const std::vector<std::vector<T>>& g, int x, int y) {
        auto [wd, hgt] = get_wd_and_hgt(g);
        std::vector<std::tuple<int, int>> neighbors;
        if (x != 0) {
            neighbors.push_back({ x - 1, y });
        }
        if (y != 0) {
            neighbors.push_back({ x, y - 1 });
        }
        if (x < wd - 1) {
            neighbors.push_back({ x + 1, y });
        }
        if (y < hgt - 1) {
            neighbors.push_back({ x, y + 1 });
        }

        return neighbors;
    }

    template<typename T>
    std::vector<std::vector<T>> make_2d_array(int wd, int hgt) {
        return std::vector<std::vector<T>>(hgt, std::vector<T>(wd, T{}));
    }

    using grid_loc_t = std::tuple<int, int>;
    struct dijkstra_info_t {
        int dist;
        grid_loc_t prev;
        std::vector<grid_loc_t> adj_locs;

        dijkstra_info_t() :
            dist(std::numeric_limits<int>::max()),
            prev({ -1,-1 })
        {}
    };
    using grid_t = std::vector<std::vector<int>>;
    using dijkstra_state_t = std::vector<std::vector< dijkstra_info_t>>;

    /*
       dijkstra's algorithm requires a queue from which you can pop the minimum priority item efficiently,
       and also change the priority of items in the queue efficiently. Implement with a multimap as the
       actual prioirty queue but also keeping in sync a hash table mapping locations to multimap items to
       facillitate efficiently changing priorities by being able to delete items i.e. change priotities
       by deleting and re-inserting with a new priority.
    */

    class priority_queue_of_locs {
    private:
        using priority_map_t = std::multimap<int, grid_loc_t>;

        struct tuple_hash_t {
            std::size_t operator()(const grid_loc_t& k) const {
                return std::hash<int>{}(std::get<0>(k) ^ std::get<1>(k));
            }
        };
        using loc_to_map_t = std::unordered_map<grid_loc_t, priority_map_t::iterator, tuple_hash_t>;

        priority_map_t priority_map_;
        loc_to_map_t loc_to_map_;

    public:

        void insert(const grid_loc_t& loc, int priority) {
            auto iter = priority_map_.insert({ priority, loc });
            loc_to_map_[loc] = iter;
        }

        void remove(const grid_loc_t& loc) {
            priority_map_.erase(loc_to_map_[loc]);
            loc_to_map_.erase(loc);
        }

        void change_priority(const grid_loc_t& loc, int new_priority) {
            remove(loc);
            insert(loc, new_priority);
        }

        grid_loc_t pop() {
            auto loc = priority_map_.begin()->second;
            remove(loc);
            return loc;
        }

        bool contains(const grid_loc_t& loc) const {
            return loc_to_map_.find(loc) != loc_to_map_.end();
        }

        bool empty() const {
            return loc_to_map_.empty();
        }
    };

    std::vector<grid_loc_t> shortest_path_from_dijkstra_state(const std::vector<std::vector<dijkstra_info_t>>& dijkstra_state) {
        std::vector<grid_loc_t> path;
        auto [wd, hgt] = get_wd_and_hgt(dijkstra_state);
        grid_loc_t loc = { wd - 1, hgt - 1 };
        while (loc != grid_loc_t{ 0,0 }) {
            path.push_back(loc);
            auto [x, y] = loc;
            loc = dijkstra_state[y][x].prev;
        }
        //path.push_back({ 0,0 });
        std::reverse(path.begin(), path.end());
        return path;
    }

    priority_queue_of_locs initialize_priority_queue(const dijkstra_state_t& state) {
        priority_queue_of_locs unvisited;
        auto [wd, hgt] = get_wd_and_hgt(state);
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                unvisited.insert({ x,y }, state[y][x].dist);
            }
        }
        return unvisited;
    }

    dijkstra_state_t initialize_dijkstra_state(const grid_t& grid) {
        auto [wd, hgt] = get_wd_and_hgt(grid);
        auto dijkstra_state = make_2d_array<dijkstra_info_t>(wd, hgt);
        dijkstra_state[0][0].dist = grid[0][0];

        // just store the cell adjacencies in the dijkstra state as a speed
        // optimization. Otherwise would need to call get_4cell in the
        // inner loop of the algorithm...
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                dijkstra_state[y][x].adj_locs = get_4cell_neighbor_locs(grid, x, y);
            }
        }

        return dijkstra_state;
    }

    std::vector<grid_loc_t> dijkstra_shortest_path(const grid_t& grid) {
        auto [wd, hgt] = get_wd_and_hgt(grid);
        auto dijkstra_state = initialize_dijkstra_state(grid);
        auto unvisited = initialize_priority_queue(dijkstra_state);

        while (!unvisited.empty()) {
            auto u = unvisited.pop();
            auto [u_x, u_y] = u;
            auto dist_u = dijkstra_state[u_y][u_x].dist;

            for (const auto& neighbor : dijkstra_state[u_y][u_x].adj_locs) {
                if (!unvisited.contains(neighbor)) {
                    continue;
                }
                auto [v_x, v_y] = neighbor;
                auto candidate_dist = dist_u + grid[v_y][v_x];
                if (candidate_dist < dijkstra_state[v_y][v_x].dist) {
                    dijkstra_state[v_y][v_x].dist = candidate_dist;
                    dijkstra_state[v_y][v_x].prev = u;
                    unvisited.change_priority(neighbor, candidate_dist);
                }
            }
        }

        return shortest_path_from_dijkstra_state(dijkstra_state);
    }

    int get_path_weight(const grid_t& grid, const std::vector<grid_loc_t>& path) {
        int sum = 0;
        for (const auto& [x, y] : path) {
            sum += grid[y][x];
        }
        return sum;
    }

    int get_weight_of_shortest_path(const grid_t& grid) {
        return get_path_weight(grid, dijkstra_shortest_path(grid));
    }

    grid_t enlarge_grid(const grid_t& grid)
    {
        auto [wd, hgt] = get_wd_and_hgt(grid);
        auto enlarged_wd = 5 * wd;
        auto enlarged_hgt = 5 * hgt;

        constexpr auto copy_tile = [](const grid_t& src, int addend, grid_t& dst, int dst_x, int dst_y) {

            constexpr auto add_and_wrap_between_1_to_8 = [](int a, int b) {
                return (((a - 1) + b) % 9) + 1;
                };

            auto [src_wd, src_hgt] = get_wd_and_hgt(src);
            for (int y = 0; y < src_hgt; y++) {
                for (int x = 0; x < src_wd; x++) {
                    dst[dst_y + y][dst_x + x] = add_and_wrap_between_1_to_8(src[y][x], addend);
                }
            }
            };

        constexpr int addend[5][5] = {
            {0,1,2,3,4},
            {1,2,3,4,5},
            {2,3,4,5,6},
            {3,4,5,6,7},
            {4,5,6,7,8}
        };

        auto large_grid = make_2d_array<int>(5 * wd, 5 * hgt);
        for (int tile_y = 0; tile_y < 5; ++tile_y) {
            for (int tile_x = 0; tile_x < 5; ++tile_x) {
                copy_tile(grid, addend[tile_y][tile_x], large_grid, wd * tile_x, hgt * tile_y);
            }
        }

        return large_grid;
    }
}

void aoc::y2021::day_15(const std::string& title) {
    auto grid = aoc::strings_to_2D_array_of_digits(
        aoc::file_to_string_vector(aoc::input_path(2021, 15))
    );

    std::println("--- Day 15: {} ---", title);
    std::println("  part 1: {}", get_weight_of_shortest_path(grid));
    std::println("  part 2: {}", get_weight_of_shortest_path(enlarge_grid(grid)));
}