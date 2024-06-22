#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <queue>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    
    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    using point_set = std::unordered_set<point, point_hash>;
    using vault_item_map = std::unordered_map<point, char, point_hash>;
    
    struct vault_contents {
        point_set hallways;
        std::vector<std::tuple<char, point>> items;
    };
    
    vault_contents parse_grid(const std::vector<std::string>& grid) {
        vault_contents contents;
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                char tile = grid[y][x];
                if (tile == '#') {
                    continue;
                }
                point loc = { x,y };
                contents.hallways.insert(loc);
                if (tile != '.') {
                    contents.items.emplace_back(tile, loc);
                }
            }
        }
        return contents;
    }

    struct tunnel {
        char item;
        int dist;
    };
    
    std::vector<tunnel> find_tunnels(
            const point_set& hallways, const point& start, const vault_item_map& items) {

        struct state {
            point loc;
            int dist;
        };

        std::vector<tunnel> tunnels;
        std::array<point, 4> neighborhood = {{ {0,-1},{1,0},{0,1},{-1,0} }};
        point_set visited;
        std::queue<state> queue;
        queue.emplace( start, 0 );

        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current.loc)) {
                continue;
            }
            visited.insert(current.loc);

            if (items.contains(current.loc)  && current.dist != 0) {
                auto item = items.at(current.loc);
                if (item != '@' && item != '$' && item != '%' && item != '&') {
                    tunnels.emplace_back(item, current.dist);
                    continue;
                }
            }

            auto neighbors = neighborhood | rv::transform(
                    [&](auto&& delta)->point {
                        return current.loc + delta;
                    }
                ) | rv::filter(
                    [&](auto&& pt) {
                        return hallways.contains(pt);
                    }
                );
            for (const auto& neighbor : neighbors) {
                queue.emplace(
                    neighbor,
                    current.dist + 1
                );
            }
        }
        return tunnels;
    }

    using vault_graph = std::unordered_map<char, std::vector<tunnel>>;

    vault_graph grid_to_graph(const std::vector<std::string>& grid) {
        vault_graph graph;
        auto contents = parse_grid(grid);
        vault_item_map item_map = contents.items | rv::transform(
                [](auto&& item)->vault_item_map::value_type {
                    auto [tile, loc] = item;
                    return { loc,tile };
                }
            ) | r::to<vault_item_map>();
        for (const auto& [item, loc] : contents.items) {
            auto tunnels = find_tunnels(contents.hallways, loc, item_map);
            for (const auto& tunnel : tunnels) {
                graph[item].push_back(tunnel);
            }
        }
        return graph;
    }

    struct traversal_state {
        char node;
        std::set<char> keys;

        bool operator==(const traversal_state& s) const {
            if (node != s.node || keys.size() != s.keys.size()) {
                return false;
            }
            for (auto [lhs, rhs] : rv::zip(keys, s.keys)) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };

    struct traversal_state_hash {
        size_t operator()(const traversal_state& state) const {
            size_t seed = 0;
            boost::hash_combine(seed, state.node);
            for (auto ch : state.keys) {
                boost::hash_combine(seed, ch);
            }
            return seed;
        }
    };

    bool is_key(char item) {
        return std::islower(item);
    }

    bool is_door(char item) {
        return std::isupper(item);
    }

    char key_for_door(char door) {
        return std::tolower(door);
    }

    std::vector<std::tuple<traversal_state, int>> neighboring_states(
            const traversal_state& u, const vault_graph& g) {
        std::array<std::vector<std::tuple<traversal_state, int>>, 2> ary;

        auto keys = g.at(u.node) | rv::filter(
            [](auto&& tunnel) {
                return is_key(tunnel.item);
            }
        );
        auto doors = g.at(u.node) | rv::filter(
            [&](auto&& tunnel) {
                return is_door(tunnel.item) && 
                    u.keys.contains(key_for_door(tunnel.item));
            }
        );

        // neighboring keys
        ary[0] = keys | rv::transform(
                [&](auto&& tunnel)->std::tuple<traversal_state, int> {
                    auto keys_possessed = u.keys;
                    keys_possessed.insert(tunnel.item);
                    traversal_state new_state = {
                        tunnel.item,
                        std::move(keys_possessed)
                    };
                    return { std::move(new_state), tunnel.dist };
                }
            ) | r::to<std::vector>();

        // neighboring doors for which we have the key.
        ary[1] = doors | rv::transform(
                [&](auto&& tunnel)->std::tuple<traversal_state, int> {
                    traversal_state new_state = {
                        tunnel.item,
                        u.keys
                    };
                    return { std::move(new_state), tunnel.dist };
                }
            ) | r::to<std::vector>();

        return ary | rv::join | r::to<std::vector>();
    }

    int find_best_solution(const vault_graph& g, const auto& distance_map) {
        int num_keys = r::count_if(g | rv::keys, is_key);
        auto all_keys = distance_map | rv::filter(
            [num_keys](auto&& state_to_dist) {
                const auto& [state, distance] = state_to_dist;
                return state.keys.size() == num_keys;
            }
        ) | r::to<std::vector>();

        auto closest = r::min_element(
            all_keys,
            [](auto&& lhs, auto&& rhs) {
                return lhs.second < rhs.second;
            }
        );

        return closest->second;
    }

    int dijkstra_shortest_path_part1(const vault_graph& g) {
        
        std::unordered_map<traversal_state, int, traversal_state_hash> dist;
        aoc::priority_queue<traversal_state, traversal_state_hash> queue;
        traversal_state start_state = { '@', {} };
        queue.insert(start_state, 0);
        dist[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto [v, dist_to_v] : neighboring_states(u, g)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + dist_to_v;
                auto curr_dist_to_v = dist.contains(v) ? dist.at(v) : std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    dist[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return find_best_solution(g, dist);
    }

    point find_start(const std::vector<std::string>& grid) {
        vault_contents contents;
        int hgt = static_cast<int>(grid.size());
        int wd = static_cast<int>(grid[0].size());
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                if (grid[y][x] == '@') {
                    return { x,y };
                }
            }
        }
        return { -1,-1 };
    }

    // use '@', '$', '%' and '&' to repesent the starting locations of
    // the four robots so we can tell them apart in the graph.

    std::vector<std::string> make_part2_grid(const std::vector<std::string>& part1) {
        auto g = part1;
        auto s = find_start(part1);

        g[s.y - 1][s.x - 1] = '@';
        g[s.y - 1][s.x] = '#';
        g[s.y - 1][s.x + 1] = '$';

        g[s.y][s.x - 1] = '#';
        g[s.y][s.x] = '#';
        g[s.y][s.x + 1] = '#';

        g[s.y + 1][s.x - 1] = '%';
        g[s.y + 1][s.x] = '#';
        g[s.y + 1][s.x + 1] = '&';

        return g;
    }

    struct multi_trav_state {
        std::array<char,4> nodes;
        std::set<char> keys;

        bool operator==(const multi_trav_state& s) const {
            if (keys.size() != s.keys.size()) {
                return false;
            }
            for (auto [lhs, rhs] : rv::zip(nodes, s.nodes)) {
                if (lhs != rhs) {
                    return false;
                }
            }
            for (auto [lhs, rhs] : rv::zip(keys, s.keys)) {
                if (lhs != rhs) {
                    return false;
                }
            }
            return true;
        }
    };

    struct multi_trav_state_hash {
        size_t operator()(const multi_trav_state& state) const {
            size_t seed = 0;
            for (auto node : state.nodes) {
                boost::hash_combine(seed, node);
            }
            for (auto key : state.keys) {
                boost::hash_combine(seed, key);
            }
            return seed;
        }
    };

    std::vector<std::tuple<multi_trav_state, int>> neighboring_multi_states(
            const multi_trav_state& u, const vault_graph& g) {

        std::vector<std::tuple<multi_trav_state, int>> output;
        for (auto [robot_index, robot] : rv::enumerate(u.nodes)) {
            traversal_state robot_state = { robot, u.keys };
            auto multi = neighboring_states(robot_state, g) |
                rv::transform(
                    [&](auto&& state_and_distance)->std::tuple<multi_trav_state, int> {
                        const auto& [s, distance] = state_and_distance;

                        multi_trav_state multi = u;
                        multi.nodes[robot_index] = s.node;
                        multi.keys = s.keys;

                        return { std::move(multi), distance };
                    }
                ) | r::to<std::vector>();
            r::copy(multi, std::back_inserter(output));
        }
        return output;
    }

    int dijkstra_shortest_path_part2(const vault_graph& g) {
        std::unordered_map<multi_trav_state, int, multi_trav_state_hash> dist;
        aoc::priority_queue<multi_trav_state, multi_trav_state_hash> queue;
        multi_trav_state start_state = { {{'@','$','%','&'}}, {} };
        queue.insert(start_state, 0);
        dist[start_state] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto [v, dist_to_v] : neighboring_multi_states(u, g)) {
                auto dist_to_u = dist.at(u);
                auto dist_through_u_to_v = dist_to_u + dist_to_v;
                auto curr_dist_to_v = dist.contains(v) ? dist.at(v) : std::numeric_limits<int>::max();

                if (dist_through_u_to_v < curr_dist_to_v) {
                    dist[v] = dist_through_u_to_v;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    }
                    else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }

        return find_best_solution(g, dist);
    }
}

void aoc::y2019::day_18(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2019, 18));
    auto part1_graph = grid_to_graph(inp);

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}",
        dijkstra_shortest_path_part1(part1_graph)
    );

    auto part2_grid = make_part2_grid(inp);
    auto part2_graph = grid_to_graph(part2_grid);

    std::println("  part 2: {}",
        dijkstra_shortest_path_part2(part2_graph)
    );
}