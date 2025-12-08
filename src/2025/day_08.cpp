
#include "../util/util.h"
#include "y2025.h"
#include <filesystem>
#include <format>
#include <print>
#include <stack>
#include <ranges>
#include <unordered_set>
#include "../util/vec3.h"

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec3<int64_t>;
    using point_set = aoc::vec3_set<int64_t>;

    class clustering {
        aoc::vec3_map<int64_t, std::vector<point>> graph_;

        std::vector<point> find_cluster(const point& s) const {
            std::stack<point> stack;
            point_set visited;

            stack.push(s);
            while (!stack.empty()) {
                auto curr = stack.top();
                stack.pop();
                
                if (visited.contains(curr)) {
                    continue;
                }
                visited.insert(curr);

                for (const auto& adj : graph_.at(curr)) {
                    stack.push(adj);
                }
            }

            return visited | r::to<std::vector>();
        }

    public:

        void connect(const point& u, const point& v) {
            graph_[u].push_back(v);
            graph_[v].push_back(u);
        }

        std::vector<std::vector<point>> clusters() const {
            std::vector<std::vector<point>> output;
            point_set seen;
            for (const auto& u : graph_ | rv::keys) {
                if (seen.contains(u)) {
                    continue;
                }
                auto cluster = find_cluster(u);
                for (const auto& member : cluster) {
                    seen.insert(member);
                }
                output.push_back(cluster);
            }
            return output;
        }

    };

    struct pair {
        std::size_t i;
        std::size_t j;
        int64_t dist2; 
    };

    int64_t sqdist(const point& a, const point& b) {
        int64_t dx = a.x - b.x;
        int64_t dy = a.y - b.y;
        int64_t dz = a.z - b.z;
        return dx * dx + dy * dy + dz * dz;
    }

    std::vector<pair> closest_pairs_bruteforce( const std::vector<point>& pts,
            std::optional<std::size_t> k = std::nullopt) {

        auto n = pts.size();
        std::vector<pair> results;
        results.reserve(n * (n - 1) / 2);

        // Compute all pairs
        for (std::size_t i = 0; i < n; ++i) {
            for (std::size_t j = i + 1; j < n; ++j) {
                results.push_back({ i, j, sqdist(pts[i], pts[j]) });
            }
        }

        std::ranges::sort(results,
            [](auto const& a, auto const& b) {
                return a.dist2 < b.dist2;
            }
        );

        if (!k.has_value()) {
            return results;
        }

        if (*k < results.size()) {
            results.resize(*k);
        }

        return results;
    }

    int64_t do_part_1(const std::vector<point>& inp, int k) {
        auto closest = closest_pairs_bruteforce(inp, k);

        clustering clust;
        for (const auto& p : closest) {
            clust.connect(inp[p.i], inp[p.j]);
        }
        auto sizes = clust.clusters() | rv::transform(
            [](auto&& c)->int { return c.size(); }
        ) | r::to<std::vector>();
        r::sort(sizes, r::greater{});

        return r::fold_left(sizes | rv::take(3), 1, std::multiplies<int64_t>{});
    }

    int64_t do_part_2(const std::vector<point>& inp) {
        auto n = inp.size();
        auto closest = closest_pairs_bruteforce(inp);

        clustering clust;
        for (const auto& p : closest) {
            clust.connect(inp[p.i], inp[p.j]);

            auto clusters = clust.clusters();
            if (clusters.size() == 1 && clusters.front().size() == n) {
                return inp[p.i].x * inp[p.j].x;
            }
        }

        return 0;
    }

}

void aoc::y2025::day_08(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2025, 8)
        ) | rv::transform(
            [](auto&& str)->point {
                auto parts = aoc::extract_numbers_int64(str);
                return { parts[0], parts[1], parts[2] };
            }
        ) | r::to<std::vector>();

    std::println("--- Day 8: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp, 1000));
    std::println("  part 2: {}", do_part_2(inp) );
    
}
