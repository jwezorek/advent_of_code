
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using graph = std::unordered_map<std::string, std::vector<std::string>>;
    using edge = std::tuple<std::string, std::string>;

    graph make_graph(auto&& edges) {
        graph g;
        for (const auto& [u, v] : edges) {
            g[u].push_back(v);
            g[v].push_back(u);
        }
        return g;
    }

    edge str_to_edge(const std::string& str) {
        return aoc::split_to_tuple<2>(str, '-');
    }

    using triple = std::array<std::string, 3>;

    std::vector<triple> three_cycles(const graph& g) {

        enum color {
            white,
            gray,
            black
        };

        using color_map = std::unordered_map<std::string, color>;
        color_map vert_color = g | rv::keys | rv::transform(
                [](auto&& u)->color_map::value_type {
                    return { u, white };
                }
            ) | r::to<color_map>();

        std::set<triple> cycles;

        auto dfs = [&](this auto&& self, const std::string& u, const std::string& parent)->void {
            vert_color[u] = gray;
            for (const auto& v : g.at(u)) {
                if (vert_color[v] == white) {
                    self(v, u);
                } else if (vert_color[v] == gray && v != parent) {
                    for (const auto& adj : g.at(v)) {
                        if (adj == u) {
                            continue;
                        }
                        auto i = r::find(g.at(u), adj);
                        if (i != g.at(u).end()) {
                            triple cycle = {{ u, v, adj }};
                            r::sort(cycle);
                            cycles.insert(cycle);
                        }
                    }
                }
            }
            vert_color[u] = black;
        };

        for (const auto& u : g | rv::keys) {
            dfs(u, {});
        }

        return cycles | r::to<std::vector>();
    }

    // Bron-Kerbosch algorithm with pivoting
    void bron_kerbosch_pivot( std::unordered_set<std::string> R,  std::unordered_set<std::string> P,
            std::unordered_set<std::string> X, const graph& g,
            std::vector<std::vector<std::string>>& maximal_cliques ) {

        if (P.empty() && X.empty()) {
            maximal_cliques.emplace_back( R | r::to<std::vector>() );
            return;
        }

        // Choose a pivot as the vertex in P with the largest neighborhood...
        std::string pivot;
        if (!P.empty()) {
            pivot = r::max(P,
                [&](auto&& lhs, auto&& rhs) {
                    return g.at(lhs).size() < g.at(rhs).size();
                }
            );
        }

        // P - neighborhood(pivot)...
        std::unordered_set<std::string> P_without_neighbors;
        for (const auto& v : P) {
            if (r::find(g.at(pivot), v) == g.at(pivot).end()) {
                P_without_neighbors.insert(v);
            }
        }

        for (const auto& v : P_without_neighbors) {
            // union(R , {v})...
            auto R_new = R;
            R_new.insert(v);

            // intersect(P , neighborhood(v))
            std::unordered_set<std::string> P_new;
            for (const auto& u : P) {
                if (r::find(g.at(v), u) != g.at(v).end()) {
                    P_new.insert(u);
                }
            }

            // intersect(X , neighborhood(v))...
            std::unordered_set<std::string> X_new;
            for (const auto& u : X) {
                if (r::find(g.at(v), u) != g.at(v).end()) {
                    X_new.insert(u);
                }
            }

            bron_kerbosch_pivot(R_new, P_new, X_new, g, maximal_cliques);

            // P - {v}, union(X , {v})...
            P.erase(v);
            X.insert(v);
        }
    }

    std::string find_largest_clique(const graph& g) {
        std::unordered_set<std::string> R;
        std::unordered_set<std::string> P = g | rv::keys | r::to<std::unordered_set>();
        std::unordered_set<std::string> X;

        std::vector<std::vector<std::string>> maximal_cliques;

        bron_kerbosch_pivot(R, P, X, g, maximal_cliques);

        auto maximal_clique = r::max(
            maximal_cliques,
            [](auto&& lhs, auto&& rhs) {
                return lhs.size() < rhs.size();
            }
        );
        r::sort(maximal_clique);

        return maximal_clique | rv::join_with(',') | r::to<std::string>();
    }
}

void aoc::y2024::day_23(const std::string& title) {

    auto graph = make_graph(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 23)
        ) | rv::transform(
            str_to_edge
        )
    );

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", 
        r::count_if(
            three_cycles(graph),
            [](auto&& cycle) {
                return !(
                    r::find_if(
                        cycle, 
                        [](auto&& v) {
                            return v[0] == 't'; 
                        }
                    ) == cycle.end());
            }
        )
    );

    std::println("  part 2: {}", find_largest_clique(graph) );
    
}
