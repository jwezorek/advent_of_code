
#include "../util/util.h"
#include "../util/vec2.h"
#include "y2025.h"
#include <filesystem>
#include <stack>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec2<int>;
    using point_map = aoc::vec2_map<int, int>;
    using graph = std::vector<std::vector<int>>;

    point_map splitter_locations(const std::vector<std::string>& inp) {
        int wd = static_cast<int>(inp.front().size());
        int hgt = static_cast<int>(inp.size());
        point start = { -1,-1 }; 
        point_map splitters;

        int id = 0;
        for (auto y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                auto ch = inp[y][x];
                if (ch == 'S') {
                    splitters[point{ x,y }] = id++;
                    continue;
                }
                if (ch == '^') {
                    splitters[point{ x,y }] = id++;
                }
            }
        }

        return splitters;
    }

    std::optional<int> next_splitter(const point& loc, const point_map& loc_to_id, int hgt) {
        auto u = loc;
        while (u.y < hgt && !loc_to_id.contains(u)) {
            u = u + point{ 0,1 };
        }
        if (loc_to_id.contains(u)) {
            return loc_to_id.at(u);
        }
        return {};
    }

    graph build_graph(const std::vector<std::string>& inp) {
        point_map loc_to_id = splitter_locations(inp);
        int hgt = static_cast<int>(inp.size());

        int n = loc_to_id.size(); // vertex n will be the sink...
        graph g(n+1);
        for (const auto& [loc, id] : loc_to_id) {
            if (id == 0) {
                // the src just has one child directly below it...
                auto next = next_splitter(loc + point{0,1}, loc_to_id, hgt);
                g[id].push_back(*next);
                continue;
            }
            g[id].push_back(next_splitter(loc - point{ 1,0 }, loc_to_id, hgt).value_or(n));
            g[id].push_back(next_splitter(loc + point{ 1,0 }, loc_to_id, hgt).value_or(n));
        }

        return g;
    }

    int reachable_vertices(const graph& dag) {
        std::stack<int> stack;
        std::unordered_set<int> visited;

        stack.push(0);
        while (!stack.empty()) {
            auto curr = stack.top();
            stack.pop();
            
            if (visited.contains(curr)) {
                continue;
            }
            visited.insert(curr);

            for (const auto& adj : dag[curr]) {
                stack.push(adj);
            }
        }

        return visited.size();
    }

    int64_t count_paths_in_dag(const graph& dag) {
        std::unordered_map<int, int64_t> memos;

        auto count_paths_aux = [&](this const auto& self, int u) {
            if (memos.contains(u)) {
                return memos.at(u);
            }

            const auto& adj_list = dag[u];
            auto sum = (adj_list.empty()) ? int64_t{ 1 } :
                r::fold_left(
                    adj_list | rv::transform(
                        [&](int adj)->int64_t {
                            return self(adj);
                        }
                    ),
                    int64_t{ 0 },
                    std::plus<int64_t>()
                );

            memos[u] = sum;
            return sum;
        };

        return count_paths_aux(0);
    }
}


void aoc::y2025::day_07(const std::string& title) {

    auto g = build_graph(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 7)
        )
    );

    std::println("--- Day 7: {} ---", title);

    // -2 in part 1, because the src and sink are not splitters
    std::println("  part 1: {}", reachable_vertices(g) - 2); 
    std::println("  part 2: {}", count_paths_in_dag(g) );
    
}
