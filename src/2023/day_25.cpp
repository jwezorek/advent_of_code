#include "y2023.h"
#include "../util/util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <queue>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    int random_number(int n) {
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, n-1);
        return dist(rng);
    }

    std::tuple<std::string, std::string> order(const std::string& u, const std::string& v) {
        if (u < v) {
            return { u,v };
        } else {
            return { v,u };
        }
    }

    class graph {

        struct edge {
            std::string label;
            std::string u;
            std::string v;
        };

        using edge_list = std::vector<std::shared_ptr<edge>>;
        struct node {
            std::string label;
            edge_list edges;
        };

        std::unordered_map<std::string, node> nodes_;
        int index_;

        std::tuple<std::string, std::string> random_edge() const {
            int u = random_number(nodes_.size());
            auto iter = nodes_.begin();
            for (int i = 0; i < u; ++i) {
                ++iter;
            }
            const auto& u_edges = iter->second.edges;
            const auto& edge = u_edges.at(random_number(u_edges.size()));
            return order( edge->u, edge->v );
        }

        int find_connected_component(std::unordered_set<std::string>& visited, const std::string& start) const {
            std::queue<std::string> queue;
            queue.push(start);
            int sz = 0;
            while (!queue.empty()) {
                auto u = queue.front();
                queue.pop();
                if (visited.contains(u)) {
                    continue;
                }
                sz++;
                visited.insert(u);
                for ( auto e : nodes_.at(u).edges) {
                    queue.push(e->u);
                    queue.push(e->v);
                }
            }
            return sz;
        }

        graph( const std::unordered_map<std::string, node>& nodes, int index) : index_(index) {
            nodes_ = nodes | rv::transform(
                    [](auto&& itm)->std::unordered_map<std::string, node>::value_type {
                        const auto& [key, v] = itm;
                        return { key, node{ v.label, edge_list{}} };
                    }
                ) | r::to<std::unordered_map<std::string, node>>();
            for (const auto& [u, vs] : nodes ) {
                for (auto e : vs.edges) {
                    insert_edge(e->u, e->v, e->label);
                }
            }
        }

    public:
        graph() : index_(0) {}

        void insert_node(const std::string& u) {
            if (nodes_.contains(u)) {
                return;
            }
            nodes_.emplace( u, node{ u, edge_list{} });
        }

        bool has_edge(const std::string& u, const std::string& v) {
            if (!nodes_.contains(u) || !nodes_.contains(v)) {
                return false;
            }
            const auto& u_edges = nodes_.at(u).edges;
            return r::find_if(
                u_edges,
                [v](auto e) {
                    return e->u == v || e->v == v;
                }
            ) != u_edges.end();
        }

        void insert_edge(const std::string& node1, const std::string& node2, 
                const std::string& lbl = {}) {
            auto [u, v] = order(node1, node2);
            if (has_edge(u, v)) {
                return;
            }
            auto edge_label = lbl.empty() ? node1 + "-" + node2 : lbl;
            auto new_edge = std::make_shared<edge>(edge_label, u, v);
            nodes_.at(u).edges.push_back(new_edge);
            nodes_.at(v).edges.push_back(new_edge);
        }

        void contract_random_edge() {
            auto [u, v] = random_edge();
            auto not_uv = [u, v](auto e) {
                return e->u != u || e->v != v;
            };
            edge_list combined_edges;
            const auto& u_edges = nodes_.at(u).edges;
            r::copy(u_edges | rv::filter(not_uv),
                std::back_inserter(combined_edges)
            );
            const auto& v_edges = nodes_.at(v).edges;
            r::copy(v_edges | rv::filter(not_uv),
                std::back_inserter(combined_edges)
            );

            auto new_node_name = std::format("node-{}", index_++);
            for (auto& e : combined_edges) {
                if (e->u == u || e->u == v) {
                    std::tie(e->u, e->v) = order(new_node_name, e->v);
                }
                if (e->v == v || e->v == u) {
                    std::tie(e->u, e->v) = order(new_node_name, e->u);
                }
            }

            nodes_.erase(u);
            nodes_.erase(v);
            nodes_[new_node_name] = node{ new_node_name, std::move(combined_edges) };
        }

        size_t size() const {
            return nodes_.size();
        }

        std::vector<std::string> edges() const {
            auto set = nodes_ | rv::transform(
                    [](auto&& itm) {
                        return itm.second.edges | rv::transform(
                            [](auto&& e) {
                                return e->label;
                            }
                        );
                    }
                ) | rv::join | r::to<std::unordered_set<std::string>>();
            return set | r::to<std::vector<std::string>>();
        }

        graph clone() const {
            return ::graph(nodes_, index_);
        }

        void remove_edge(const std::string& node1, const std::string& node2) {
            auto [u, v] = order(node1, node2);

            auto not_uv = [u, v](auto e) {
                return e->u != u || e->v != v;
            };

            auto& node_u = nodes_[u];
            node_u.edges = nodes_[u].edges |
                rv::filter(not_uv) | r::to<edge_list>();

            auto& node_v = nodes_[v];
            node_v.edges = nodes_[v].edges |
                rv::filter(not_uv) | r::to<edge_list>();
        }

        std::vector<int> connect_components_size() const {
            std::unordered_set<std::string> visited;
            std::vector<int> conn_comps;
            for (auto node_name : nodes_ | rv::keys) {
                if (visited.contains(node_name)) {
                    continue;
                }
                conn_comps.push_back(
                    find_connected_component(visited, node_name)
                );
            }
            return conn_comps;
        }
    };

    std::vector<std::string> nodes_from_input(const std::vector<std::string>& inp) {
        auto lines = inp | rv::transform(
            [](auto&& str) {
                return aoc::extract_alphabetic(str);
            }
        ) | r::to<std::vector<std::vector<std::string>>>();
        return lines | rv::join | r::to<std::unordered_set<std::string>>() |
            r::to<std::vector<std::string>>();
    }

    graph parse_input(const std::vector<std::string>& inp) {
        ::graph graph;
        auto nodes = nodes_from_input(inp);
        for (auto node : nodes) {
            graph.insert_node(node);
        }
        for (const auto& line : inp) {
            auto nodes = aoc::extract_alphabetic(line);
            auto u = nodes.front();
            for (auto v : nodes | rv::drop(1)) {
                graph.insert_edge(u, v);
            }
        }

        return graph;
    }

    std::vector<std::string> kargers_algorithm(const graph& g, int k) {
        std::vector<std::string> edges;
        while (edges.size() != k) {
            auto graph = g.clone();
            while (graph.size() > 2) {
                graph.contract_random_edge();
            }
            edges = graph.edges();
        }
        return edges;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_25(const std::string& title) {
    auto graph = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 25)));

    std::println("--- Day 25: {0} ---\n", title);

    auto edges = kargers_algorithm(graph, 3);
    for (auto e : edges) {
        auto [u, v] = aoc::split_to_tuple<2>(e, '-');
        graph.remove_edge(u, v);
    }
    auto comps = graph.connect_components_size();
    std::println("  part 1: {}", comps.front() * comps.back());
}