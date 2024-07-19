#include "../util/util.h"
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

    struct vertex {
        std::string name;
        std::vector<vertex*> adjacencies;

        vertex()
        {}

        vertex(const std::string& v) :
            name(v)
        { }

        bool is_end() const {
            return name == "end";
        }

        bool is_small() const {
            return std::islower(name[0]);
        }
    };

    class graph {

        using edge = std::tuple<std::string, std::string>;

        static std::vector<edge> input_to_edges(const std::vector<std::string>& input) {
            std::vector<edge> edges(input.size());
            std::transform(input.begin(), input.end(), edges.begin(),
                [](const std::string& str)->edge {
                    auto u_v = aoc::split(str, '-');
                    return { u_v[0],u_v[1] };
                }
            );
            return edges;
        }

        void init_vertices(const std::vector<edge>& edges) {
            std::unordered_set<std::string> vertex_names;
            for (const auto& [u, v] : edges) {
                vertex_names.insert(u);
                vertex_names.insert(v);
            }
            for (const auto& v : vertex_names) {
                vertices_[v] = vertex(v);
            }
        }

        void insert_edges(const std::vector<edge>& edges) {
            for (const auto& [u_str, v_str] : edges) {
                auto& u = vertices_[u_str];
                auto& v = vertices_[v_str];
                u.adjacencies.push_back(&v);
                v.adjacencies.push_back(&u);
            }
        }

        std::unordered_map<std::string, vertex> vertices_;

    public:

        graph(const std::vector<std::string>& input) {
            auto edges = input_to_edges(input);
            init_vertices(edges);
            insert_edges(edges);
        }

        const vertex* vert(const std::string& v) const {
            return &(vertices_.at(v));
        }
    };

    class part1_visited_set {
    private:
        std::unordered_set<std::string> visited_;
    public:
        bool can_visit(const std::string& str) {
            return visited_.find(str) == visited_.end();
        }
        void insert(const std::string& str) {
            visited_.insert(str);
        }
        void remove(const std::string& str) {
            visited_.erase(str);
        }
    };

    class part2_visited_set {
    private:
        std::unordered_map<std::string, int> visited_;
        bool has_a_twoer;
    public:
        part2_visited_set() :
            has_a_twoer(false)
        {}

        bool can_visit(const std::string& str) {
            if (str == "start") {
                return false;
            }

            if (visited_.find(str) == visited_.end()) {
                return true;
            }

            if (visited_[str] == 1 && !has_a_twoer) {
                return true;
            }
            return false;
        }

        void insert(const std::string& str) {
            if (visited_.find(str) == visited_.end()) {
                visited_[str] = 1;
                return;
            }
            else {
                visited_[str]++;
                has_a_twoer = true;
            }
        }

        void remove(const std::string& str) {
            if (visited_.find(str) == visited_.end()) {
                return;
            }
            int new_count = --visited_[str];
            if (new_count == 0) {
                visited_.erase(str);
            }
            else {
                has_a_twoer = false;
            }
        }
    };

    template <typename T>
    class traversal {
    private:

        struct node {
            const vertex* vert;
            int index;

            node(const vertex* v) :
                vert(v), index(-1)
            {}
        };

        const graph& graph_;
        T visited_;
        std::vector<node> path_;

        std::optional<int> next_neighbor(const node& node) {
            const auto& adj = node.vert->adjacencies;
            for (int i = node.index + 1; i != adj.size(); ++i) {
                if (visited_.can_visit(adj[i]->name)) {
                    return i;
                }
            }
            return std::nullopt;
        }

        std::string current_path_string() {
            std::stringstream ss;
            for (int i = 0; i < path_.size(); ++i) {
                ss << path_[i].vert->name;
                if (i != path_.size() - 1) {
                    ss << ",";
                }
            }
            return ss.str();
        }

        std::string do_one_step() {

            node& current_node = path_.back();
            if (current_node.vert->is_end()) {
                std::string full_path = current_path_string();
                backtrack();
                return full_path;
            }

            auto next = next_neighbor(current_node);
            if (!next) {
                backtrack();
            }
            else {
                current_node.index = *next;
                const vertex* next_vert = current_node.vert->adjacencies[current_node.index];
                if (next_vert->is_small()) {
                    visited_.insert(next_vert->name);
                }
                path_.push_back(node(next_vert));
            }

            return {};
        }

        void backtrack() {
            const auto* v = path_.back().vert;
            if (v->is_small()) {
                visited_.remove(v->name);
            }
            path_.pop_back();
        }

    public:
        traversal(const graph& g) :
            graph_(g)
        {
            path_.push_back(g.vert("start"));
            visited_.insert("start");
        }

        std::vector<std::string> all_paths() {
            std::vector<std::string> paths;
            while (!path_.empty()) {
                auto path_str = do_one_step();
                if (!path_str.empty()) {
                    paths.push_back(path_str);
                }
            }
            return paths;
        }

    };
}

void aoc::y2021::day_12(const std::string& title) {
    graph g(aoc::file_to_string_vector(aoc::input_path(2021, 12)));

    std::println("--- Day 12: {} ---", title);
    traversal<part1_visited_set> t1(g);
    std::println("  part 1: {}", t1.all_paths().size());

    traversal<part2_visited_set> t2(g);
    std::println("  part 2: {}", t2.all_paths().size());
}