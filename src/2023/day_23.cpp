#include "y2023.h"
#include "../util/util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <queue>
#include <stack>
#include <memory>
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
    
    template<typename T>
    using loc_map = std::unordered_map<loc, T, loc_hash>;

    struct edge {
        int distance;
        int dest;
    };

    struct node {
        loc pos;
        int index;
        std::vector<edge> neighbors;
    };
    using graph = std::vector<node>;

    std::tuple<int, int> dimensions(const grid& g) {
        return {
            static_cast<int>(g.front().size()),
            static_cast<int>(g.size())
        };
    }

    bool is_slope(char ch) {
        return ch == '^' || ch == '<' || ch == 'v' || ch == '>';
    }

    loc_map<node> get_nodes(const grid& inp) {
        auto [cols, rows] = dimensions(inp);
        loc_map<node> nodes;
        int index = 0;
        nodes.insert({ loc{ 1,-1 }, node{ {1,-1}, index++, {} } });
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                auto ch = inp[row][col];
                if (is_slope(ch)) {
                    nodes[{col, row}] = { {col,row}, index++, {} };
                }
            }
        }
        nodes[{cols-2, rows}] = { {cols - 2, rows}, index, {} };
        return nodes;
    }

    loc slide_destination(const grid& inp, const loc& pos) {
        auto slope = inp[pos.row][pos.col];
        static const std::unordered_map<char, loc> offset = {
            {'^', {0,-1}}, {'<', {-1,0}},  {'v', {0,1}}, {'>',{1,0}}
        };
        return pos + offset.at(slope);
    }

    loc first_cell(const grid& inp, const node& node) {
        if (node.pos == loc{1, -1}) {
            return { 1,0 };
        }
        return slide_destination(inp, node.pos);
    }

    struct state {
        loc pos;
        int dist;
    };

    std::vector<loc> neighbors(const grid& grid, const loc& pos, bool part1 = true) {
        auto [cols, rows] = dimensions(grid);
        static const std::array<loc, 4> offsets = {{ {0,-1}, {-1,0}, {0,1}, {1,0} }};
        return offsets |
            rv::transform(
                [&](auto&& offset) {
                    return pos + offset;
                }
            ) | rv::filter(
                [&](auto&& p) {
                    if (part1) {
                        if (p == loc{ cols - 2, rows }) {
                            return true;
                        }
                    }
                    if (p.col < 0 || p.col >= cols || p.row < 0 || p.row >= rows) {
                        return false;
                    }
                    return grid[p.row][p.col] != '#';
                }
            ) | r::to<std::vector<loc>>();
    }

    std::vector<edge> find_edges(const grid& grid, const loc_map<node>& nodes, const node& node) {
        auto [cols, rows] = dimensions(grid);
        if (node.pos == loc{ cols - 2,rows }) {
            return {};
        }

        loc_set visited;
        visited.insert(node.pos);
        std::queue<state> queue;
        std::vector<edge> edges;

        queue.push({ first_cell(grid, node), 1 });
        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();
            if (visited.contains(curr_state.pos)) {
                continue;
            }
            visited.insert(curr_state.pos);
            if (curr_state.pos == loc{ cols - 2,rows }) {
                edges.emplace_back(
                    curr_state.dist,
                    nodes.at(curr_state.pos).index
                );
                continue;
            } 
            if (is_slope(grid[curr_state.pos.row][curr_state.pos.col])) {
                if (!visited.contains(slide_destination(grid, curr_state.pos))) {
                    edges.emplace_back(
                        curr_state.dist,
                        nodes.at(curr_state.pos).index
                    );
                }
                continue;
            }
            
            for (auto neighbor : neighbors(grid, curr_state.pos)) {
                queue.push(
                    { neighbor, curr_state.dist + 1 }
                );
            }
        }
        return edges;
    }

    graph parse_input(const grid& inp) {
        auto nodes = get_nodes(inp);
        for (auto& key_val : nodes) {
            key_val.second.neighbors = find_edges(inp, nodes, key_val.second);
        }
        graph g(nodes.size());
        for (const auto& node : nodes) {
            g[node.second.index] = node.second;
        }
        return g;
    }

    int longest_path(const graph& g) {

        std::vector<int> dist(g.size());
        for (auto& val : dist) {
            val = std::numeric_limits<int>::max();
        }

        std::vector<int> actual_dist(g.size());
        for (auto& val : actual_dist) {
            val = 0;
        }

        std::vector<int> pred(g.size());
        for (auto& loc : pred) {
            loc = -1;
        }

        aoc::priority_queue<int> queue;
        int start = g.front().index;
        queue.insert(start,0);
        dist[0] = 0;

        while (!queue.empty()) {
            auto u = queue.extract_min();
            for (auto [u_to_v_raw, v] : g.at(u).neighbors) {
                auto dist_to_u = dist.at(u);
                auto u_to_v = -u_to_v_raw;
                auto dist_through_u_to_v = dist_to_u + u_to_v;
                auto curr_dist_to_v = dist.at(v);
                if (dist_through_u_to_v < curr_dist_to_v) {
                    dist[v] = dist_through_u_to_v;
                    pred[v] = u;
                    actual_dist[v] = actual_dist[u] + u_to_v_raw;
                    if (queue.contains(v)) {
                        queue.change_priority(v, dist_through_u_to_v);
                    } else {
                        queue.insert(v, dist_through_u_to_v);
                    }
                }
            }
        }
        return actual_dist.back()-2;
    }

    int count_open_neighbors(const grid& inp, const loc& p) {
        auto neigh = neighbors(inp, p, false);
        return neigh.size();
    }

    loc_map<node> get_nodes_part2(const grid& inp) {
        auto [cols, rows] = dimensions(inp);
        loc_map<node> nodes;
        int index = 0;
        nodes[{1, 0}] = node{ {1,0}, index++, {} };
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (inp[row][col] != '#' && count_open_neighbors(inp, { col,row }) > 2) {
                    nodes[{col,row}] = node{ {col,row}, index++, {} };
                }
            }
        }
        nodes[{cols - 2, rows-1}] = { {cols - 2, rows-1}, index, {} };
        return nodes;
    }

    bool has_edge(const node& u, int v) {
        return r::find_if(u.neighbors,
            [v](auto&& e) {
                return e.dest == v;
            }
        ) != u.neighbors.end();
    }

    void find_edges_part2(const grid& grid, loc_map<node>& nodes, node& node) {
        auto [cols, rows] = dimensions(grid);

        loc_set visited;
        std::queue<state> queue;

        queue.push({ node.pos, 0 });
        while (!queue.empty()) {
            auto curr_state = queue.front();
            queue.pop();
            if (visited.contains(curr_state.pos)) {
                continue;
            }
            visited.insert(curr_state.pos);
            if (curr_state.pos != node.pos && nodes.contains(curr_state.pos)) {
                if (!has_edge(nodes.at(curr_state.pos), node.index)) {
                    node.neighbors.emplace_back(
                        curr_state.dist,
                        nodes.at(curr_state.pos).index
                    );
                    nodes.at(curr_state.pos).neighbors.emplace_back(
                        curr_state.dist,
                        node.index
                    );
                }
                continue;
            }

            for (auto neighbor : neighbors(grid, curr_state.pos)) {
                queue.push(
                    { neighbor, curr_state.dist + 1 }
                );
            }
        }
    }

    graph part2_graph(const grid& inp) {
        auto nodes = get_nodes_part2(inp);
        for (auto& key_val : nodes) {
            find_edges_part2(inp, nodes, key_val.second);
        }
        graph g(nodes.size());
        for (const auto& node : nodes) {
            g[node.second.index] = node.second;
        }
        return g;
    }

    struct part2_state {
        std::vector<int> path;
        int length;
    };
    
    int longest_path_undirected(const graph& graph) {
        std::stack<std::shared_ptr<part2_state>> stack;
        stack.push(std::make_shared<part2_state>(std::vector{ 0 }, 0));

        int longest = 0;

        while (!stack.empty()) {
            auto state = stack.top();
            stack.pop();

            int u = state->path.back();
            if (u == graph.size() - 1) {
                if (state->length > longest) {
                    longest = state->length;
                }
                continue;
            }

            auto visited = state->path | r::to<std::unordered_set<int>>();
            for (auto [dist, v] : graph.at(u).neighbors) {
                if (visited.contains(v)) {
                    continue;
                }
                auto new_path = state->path;
                new_path.push_back(v);
                stack.push(std::make_shared<part2_state>(new_path, state->length + dist));
            }
        }

        return longest;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_23(const std::string& title) {

    auto input = aoc::file_to_string_vector(aoc::input_path(2023, 23));
    auto digraph = parse_input( input );

    std::println("--- Day 23: {0} ---\n", title);
    std::println("  part 1: {}", longest_path(digraph));
    std::println("  part 2: {}", longest_path_undirected(part2_graph(input)));
}