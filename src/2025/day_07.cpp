
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

        graph g(loc_to_id.size());
        for (const auto& [loc, id] : loc_to_id) {
            if (id == 0) {
                auto next = next_splitter(loc + point{0,1}, loc_to_id, hgt);
                g[id].push_back(*next);
                continue;
            }
            auto left = next_splitter(loc - point{ 1,0 }, loc_to_id, hgt);
            if (left) {
                g[id].push_back(*left);
            }
            auto right = next_splitter(loc + point{ 1,0 }, loc_to_id, hgt);
            if (right) {
                g[id].push_back(*right);
            }
        }

        return g;
    }

    int count_reachable_splitters(const graph& g) {
        std::stack<int> stack;
        std::unordered_set<int> visited;

        int count = 0;
        stack.push(0);
        while (!stack.empty()) {
            auto curr = stack.top();
            stack.pop();
            
            if (visited.contains(curr)) {
                continue;
            }
            visited.insert(curr);

            if (curr != 0) {
                ++count;
            }

            for (const auto& adj : g[curr]) {
                stack.push(adj);
            }
        }

        return count;
    }

    int64_t count_paths_aux(const graph& g, int u, std::unordered_map<int, int64_t>& memos) {

        if (memos.contains(u)) {
            return memos.at(u);
        }

        const auto& adj_list = g[u];
        int n = adj_list.size();
        int64_t sum = 0;

        if (n == 0) {
            sum = 2;
        } else if (n == 1) {
            int other_branch = (u == 0) ? 0 : 1;
            sum = other_branch + count_paths_aux(g, adj_list[0], memos);
        } else if (n == 2) {
            sum = count_paths_aux(g, adj_list[0], memos) +
                count_paths_aux(g, adj_list[1], memos);
        }

        memos[u] = sum;
        return sum;
    }

    int64_t count_paths(const graph& g) {
        std::unordered_map<int, int64_t> memos;
        return count_paths_aux(g, 0, memos);
    }
}


void aoc::y2025::day_07(const std::string& title) {

    auto g = build_graph(
        aoc::file_to_string_vector(
            aoc::input_path(2025, 7)
        )
    );

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", count_reachable_splitters(g));
    std::println("  part 2: {}", count_paths(g) );
    
}
