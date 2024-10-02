
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    class edge {
        int u_;
        int v_;
    public:
        edge(int u, int v) : u_(u), v_(v) {
            if (u_ > v_) {
                std::swap(u_, v_);
            }
        }
        bool operator==(const edge& e) const {
            return u_ == e.u_ && v_ == e.v_;
        }

        int u() const {
            return u_;
        }

        int v() const {
            return v_;
        }
    };

    struct hash_edge {
        size_t operator()(const edge& e) const {
            size_t seed = 0;
            boost::hash_combine(seed, e.u());
            boost::hash_combine(seed, e.v());
            return seed;
        }
    };

    using edge_set = std::unordered_set<edge, hash_edge>;

    edge parse_edge(const std::string& str) {
        auto pieces = aoc::extract_numbers(str);
        return {
            pieces.front(),
            pieces.back()
        };
    }

    struct adj_list_item {
        int dest;
        int weight;
    };

    using graph = std::unordered_map<int, std::vector<adj_list_item>>;

    graph to_graph(const std::vector<edge>& edges) {
        graph g;
        for (const auto& e : edges) {
            int weight = e.u() + e.v();
            g[e.u()].emplace_back(e.v(), weight);
            g[e.v()].emplace_back(e.u(), weight);
        }
        return g;
    }

    int max_weight_path_aux(const graph& graph, int v, edge_set& visited, int current_weight) {
        
        int max_weight = current_weight;

        for (const auto& adj : graph.at(v)) {
            edge e(v, adj.dest);
            if (!visited.contains(e)) {

                visited.insert(e);
                
                // Recur on the neighbor
                max_weight = std::max(
                    max_weight, 
                    max_weight_path_aux(
                        graph, 
                        adj.dest, 
                        visited, 
                        current_weight + adj.weight
                    )
                );


                visited.erase(e);
            }
        }
        return max_weight;

    }

    int max_weight_path(const graph& g, int start) {
        edge_set visited;
        return max_weight_path_aux(g, start, visited, 0);
    }

    int longest_path_length(const graph& inp, int start) {
        auto g = inp;
        for (auto& adj_list : g | rv::values) {
            for (auto& item : adj_list) {
                item.weight = 1;
            }
        }
        return max_weight_path(g, start);
    }

    int max_weight_long_path_aux(const graph& graph, int v, edge_set& visited, int current_weight, int curr_len, int longest_len) {
        if (curr_len == longest_len) {
            return current_weight;
        }

        int max_weight = 0;
        for (const auto& adj : graph.at(v)) {
            edge e(v, adj.dest);
            if (!visited.contains(e)) {
                visited.insert(e);

                max_weight = std::max(
                    max_weight,
                    max_weight_long_path_aux(
                        graph,
                        adj.dest,
                        visited,
                        current_weight + adj.weight,
                        curr_len + 1,
                        longest_len
                    )
                );

                visited.erase(e);
            }
        }

        return max_weight;
    }

    int max_weight_longest_path(const graph& g, int start) {
        int long_length = longest_path_length(g, start);
        edge_set visited;
        return max_weight_long_path_aux(g, start, visited, 0, 0, long_length);
    }
}

void aoc::y2017::day_24(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 24)
        ) | rv::transform(
            parse_edge
        ) | r::to<std::vector>();

    auto graph = to_graph(inp);
   

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", max_weight_path(graph, 0));
    std::println("  part 2: {}", max_weight_longest_path(graph, 0));
    
}
