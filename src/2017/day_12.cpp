
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using graph = std::unordered_map<int, std::vector<int>>;

    graph build_graph(const std::vector<std::vector<int>>& edge_lists) {
        graph g;
        for (const auto& edge_list : edge_lists) {
            auto u = edge_list.front();
            for (auto v : edge_list | rv::drop(1)) {
                g[u].push_back(v);
            }
        }
        return g;
    }

    void traverse_graph(const graph& g, std::unordered_set<int>& visited, int start) {

        std::queue<int> queue;
        queue.push(start);

        while (!queue.empty()) {
            int curr = queue.front();
            queue.pop();

            if (visited.contains(curr)) {
                continue;
            }
            visited.insert(curr);

            for (const auto neighbor : g.at(curr)) {
                queue.push(neighbor);
            }
        }
    }

    int size_of_connected_component(const graph& g, int start) {
        std::unordered_set<int> visited;
        traverse_graph(g, visited, start);
        return visited.size();
    }

    int count_connected_components(const graph& g) {
        std::unordered_set<int> visited;
        int count = 0;
        for (auto u : g | rv::keys) {
            if (visited.contains(u)) {
                continue;
            }
            ++count;
            traverse_graph(g, visited, u);
        }
        return count;
    }
}

void aoc::y2017::day_12(const std::string& title) {

    auto graph = build_graph(
        aoc::file_to_string_vector(
            aoc::input_path(2017, 12)
        ) | rv::transform(
            [](const std::string& str) {
                return aoc::extract_numbers(str);
            }
        ) | r::to<std::vector>()
    );

    std::println("--- Day 12: {} ---", title);
    std::println("  part 1: {}", size_of_connected_component(graph, 0) );
    std::println("  part 2: {}", count_connected_components(graph) );
    
}
