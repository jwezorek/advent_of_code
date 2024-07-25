
#include "../util/util.h"
#include "../util/vec2.h"
#include "../util/grid.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <queue>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using vec2 = aoc::vec2<int>;
    using grid = aoc::grid<char>;
    using point_set = aoc::vec2_set<int>;

    struct particle {
        vec2 position;
        vec2 velocity;
    };

    particle parse_particle(const std::string& inp) {
        auto vals = aoc::extract_numbers(inp, true);
        return {
            {vals[0],vals[1]},
            {vals[2],vals[3]}
        };
    }

    struct connected_component {
        vec2 seed;
        int size;
    };

    connected_component find_connected_component(
            point_set& visited, const vec2& seed, const point_set& points) {

        static const std::array<vec2, 8> deltas = {{
            {0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1}
        }};

        std::queue<vec2> queue;
        queue.push(seed);
        int count = 0;

        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();

            if (visited.contains(current)) {
                continue;
            }
            visited.insert(current);
            ++count;

            for (const auto& delta : deltas) {
                auto possible_neighbor = current + delta;
                if (points.contains(possible_neighbor)) {
                    queue.push(possible_neighbor);
                }
            }
        }

        return { seed, count };

    }

    std::vector<connected_component> find_connected_components(const point_set& points) {
        std::vector<connected_component> components;
        point_set visited;
        for (auto pt : points) {
            if (!visited.contains(pt)) {
                components.push_back(
                    find_connected_component(visited, pt, points)
                );
            }
        }
        return components;
    }

    bool seems_like_a_message(const std::vector<particle>& particles) {

        auto set = particles | rv::transform(
            [](auto&& part) {
                return part.position;
            }
        ) | r::to<point_set>();
        auto ccomps = find_connected_components(set);
        for (const auto& comp : ccomps) {
            if (comp.size < 3) {
                return false;
            }
        }
        return true;
    }

    std::tuple<int, int, int, int> bounds(const std::vector<particle>& particles) {
        auto [x1, x2] = r::minmax(
            particles | rv::transform([](auto&& p) {return p.position.x; })
        );
        auto [y1, y2] = r::minmax(
            particles | rv::transform([](auto&& p) {return p.position.y; })
        );
        return { x1,y1,x2 - x1 + 1,y2 - y1 + 1 };
    }

    std::string display_message(const std::vector<particle>& particles) {
        auto [orig_x, orig_y, wd, hgt] = bounds(particles);
        auto set = particles | rv::transform(
            [](auto&& part) {
                return part.position;
            }
        ) | r::to<point_set>();

        std::stringstream ss;
        for (auto y = 0; y < hgt; ++y) {
            ss << "  ";
            for (auto x = 0; x < wd; ++x) {
                auto pt = vec2{ orig_x + x, orig_y + y };
                auto tile = set.contains(pt) ? '#' : '.';
                ss << tile;
            }
            ss << '\n';
        }
        return ss.str();
    }

    std::tuple<int,std::string> find_message(const std::vector<particle>& particles) {
        bool done = false;
        std::vector<particle> current_state = particles;
        int steps = 0;
        while (!done) {
            ++steps;
            for (auto& p : current_state) {
                p.position = p.position + p.velocity;
            }
            done = seems_like_a_message(current_state);
        }
        return { steps, display_message(current_state) };
    }
}

void aoc::y2018::day_10(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path( 2018, 10 )
        ) | rv::transform(
            parse_particle
        ) | r::to<std::vector>();

    auto [steps, message] = find_message(inp);

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1:\n\n{}", message );
    std::println("  part 2: {}", steps);
    
}
