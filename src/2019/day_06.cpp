#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct node {
        std::string name;
        std::string parent;
        std::vector<std::string> children;
    };

    using tree = std::unordered_map<std::string, node>;

    std::tuple<std::string, std::string> orbit_edge(const std::string& str) {
        auto vec = aoc::split(str, ')');
        return { vec.front(), vec.back() };
    }

    tree build_orbit_graph(const std::vector<std::string> orbits) {
        tree g;
        for (const auto& orb_str : orbits) {
            auto [parent, child] = orbit_edge(orb_str);
            if (!g.contains(parent)) {
                g[parent] = node{ parent, {}, {} };
            }
            g[parent].children.push_back(child);
            if (!g.contains(child)) {
                g[child] = node{ child, parent, {} };
            } else {
                g[child].parent = parent;
            }
        }
        return g;
    }

    int count_orbits_aux(const tree& g, int orbits_of_parent, const std::string& u) {
        int orbits_count = orbits_of_parent + 1;
        int orbits_of_u = orbits_count;
        if (g.contains(u)) {
            for (const auto& child : g.at(u).children) {
                orbits_count += count_orbits_aux(g, orbits_of_u, child);
            }
        }
        return orbits_count;
    }

    int count_orbits(const tree& g) {
        return count_orbits_aux(g, -1, "COM");
    }

    struct state {
        int dist;
        std::string u;
    };

    int shortest_path(const tree& g, const std::string& start, const std::string& dst) {
        std::queue<state> queue;
        std::unordered_set<std::string> visited;
        queue.push({ 0,start });
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (current.u == dst) {
                return current.dist;
            }

            if (visited.contains(current.u)) {
                continue;
            }
            visited.insert(current.u);

            if (!g.at(current.u).parent.empty()) {
                queue.push(
                    { current.dist + 1, g.at(current.u).parent }
                );
            }
            for (const auto& child : g.at(current.u).children) {
                queue.push(
                    { current.dist + 1, child }
                );
            }
        }
        return -1;
    }
}

void aoc::y2019::day_06(const std::string& title) {

    auto inp = build_orbit_graph(
        aoc::file_to_string_vector(aoc::input_path(2019, 6))
    );

    std::println("--- Day 6: {} ---", title);
    std::println("  part 1: {}",
        count_orbits(inp)
    );
    std::println("  part 2: {}",
        shortest_path(inp, inp["YOU"].parent, inp["SAN"].parent)
    );
}