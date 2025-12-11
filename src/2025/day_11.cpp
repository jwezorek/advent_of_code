
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using graph = std::unordered_map<std::string, std::vector<std::string>>;

    graph parse_input(const std::vector<std::string>& inp) {
        return inp | rv::transform(
            [](auto&& str)->graph::value_type {
                auto pieces = aoc::split(str, ':');
                return {
                    pieces.front(),
                    aoc::extract_alphabetic(pieces.back())
                };
            }
        ) | r::to<graph>();
    }

    int64_t count_paths_dag(const graph& graph, const std::string& src, const std::string& sink) {

        std::unordered_map<std::string, int64_t> memo;

        auto dfs = [&](this auto&& self, const std::string& current) -> int64_t {

            if (memo.contains(current)) {
                return memo[current];
            }

            if (current == sink) {
                return 1;
            }

            int64_t total_paths = 0;
            if (graph.contains(current)) {
                const auto& adj_list = graph.at(current);
                for (const auto& adj : adj_list) {
                    total_paths += self(adj);
                }
            }

            memo[current] = total_paths;
            return total_paths;
        };

        return dfs(src);
    }

    std::string make_memo(const std::string& curr, const std::unordered_set<std::string>& inclusions) {
        auto toks = inclusions | r::to<std::vector>();
        toks.push_back(curr);
        return toks | rv::join_with(',') | r::to<std::string>();
    }

    int64_t count_paths_including_nodes(
            const graph& graph, const std::string& src, const std::string& sink,
            const std::unordered_set<std::string>& must_include) {

        std::unordered_map<std::string, int64_t> memo;

        auto dfs = [&]( this auto&& self, const std::string& current, 
                const std::unordered_set<std::string>& inclusions) -> int64_t {

            auto memo_tok = make_memo(current, inclusions);
            if (memo.contains(memo_tok)) {
                return memo[memo_tok];
            }

            if (current == sink && inclusions.size() == must_include.size()) {
                return 1;
            }

            auto new_inclusions = inclusions;
            if (must_include.contains(current)) {
                new_inclusions.insert(current);
            }

            int64_t total_paths = 0;
            if (graph.contains(current)) {
                const auto& adj_list = graph.at(current);
                for (const auto& adj : adj_list) {
                    total_paths += self(adj, new_inclusions);
                }
            }

            memo[memo_tok] = total_paths;
            return total_paths;
        };

        std::unordered_set<std::string> inclusions;
        return dfs(src, inclusions);
    }

}

void aoc::y2025::day_11(const std::string& title) {

    auto graph = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 11)
        )
    );

    std::println("--- Day 11: {} ---", title);
    std::println("  part 1: {}", count_paths_dag(graph, "you", "out"));
    std::println("  part 2: {}", 
        count_paths_including_nodes(
            graph, "svr", "out", 
            {"fft","dac"}
        )
    );
    
}
