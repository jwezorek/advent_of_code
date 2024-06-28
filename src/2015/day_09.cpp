#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <set>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::tuple<std::string, std::string> order( std::string u,  std::string v) {
        if (v < u) {
            std::swap(u, v);
        }
        return { u,v };
    }

    class edge {
        std::string u_;
        std::string v_;
    public:
        edge(const std::string& u, const std::string& v) {
            std::tie(u_, v_) = order(u, v);
        }

        std::string u() const {
            return u_;
        }
        
        std::string v() const {
            return v_;
        }

        bool operator==(const edge& e) const {
            return u_ == e.u() && v_ == e.v();
        }
    };

    struct edge_hash {
        size_t operator()(const edge& e) const {
            size_t seed = 0;
            boost::hash_combine(seed, e.u() );
            boost::hash_combine(seed, e.v() );
            return seed;
        }
    };

    using graph = std::unordered_map<edge, int, edge_hash>;

    std::tuple<edge, int> input_line_to_edge(const std::string& str) {
        auto words = aoc::split(str, ' ');
        return { { words[0], words[2] }, std::stoi(words[4]) };
    }

    graph input_to_graph(const std::vector<std::string>& inp) {
        return inp | rv::transform(
                [](auto&& str)->graph::value_type {
                    auto [edg, distance] = input_line_to_edge(str);
                    return {edg, distance};
                }
            ) | r::to<graph>();
    }

    std::vector<std::string> vertices(const graph& g) {
        std::set<std::string> vert_set;
        for (auto&& e : g | rv::keys) {
            vert_set.insert(e.u());
            vert_set.insert(e.v());
        }
        return vert_set | r::to<std::vector>();
    }

    int path_length(const std::vector<std::string>& path, const graph& g) {
        return r::fold_left(
            path | rv::slide(2) | rv::transform(
                [&](auto rng)->int {
                    edge e(rng[0], rng[1]);
                    return g.at(e);
                }
            ),
            0,
            std::plus<>()
        );
    }

    int shortest_hamiltonian_path(const graph& g) {
        auto path = vertices(g);
        int shortest_path = std::numeric_limits<int>::max();
        do {
            auto len = path_length(path, g);
            shortest_path = (len < shortest_path) ? len : shortest_path;

        } while (r::next_permutation(path).found);

        return shortest_path;
    }

    int longest_hamiltonian_path(const graph& g) {
        auto path = vertices(g);
        int longest_path = 0;
        do {
            auto len = path_length(path, g);
            longest_path = (len > longest_path) ? len : longest_path;

        } while (r::next_permutation(path).found);

        return longest_path;
    }
}

void aoc::y2015::day_09(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2015, 9));
    auto graph = input_to_graph(inp);

    std::println("--- Day 9: {} ---", title);
    std::println("  part 1: {}",
        shortest_hamiltonian_path(graph)
    );
    std::println("  part 2: {}",
        longest_hamiltonian_path(graph)
    );
}