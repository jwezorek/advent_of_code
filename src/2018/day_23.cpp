
#include "../util/util.h"
#include "../util/vec3.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using point = aoc::vec3<int64_t>;

    struct box {
        point min_pt;
        point max_pt;

        bool operator==(const box& other) const {
            return min_pt == other.min_pt && max_pt == other.max_pt;
        }
    };

    std::vector<box> subdivide_power_of_two_box(const box& box) {
        if (box.max_pt.x - box.min_pt.x == 0) {
            return {};
        }
        int sz = (box.max_pt.x - box.min_pt.x) / 2;
        int x1 = box.min_pt.x;
        int y1 = box.min_pt.y;
        int z1 = box.min_pt.z;
        int x2 = box.min_pt.x + sz;
        int y2 = box.min_pt.y + sz;
        int z2 = box.min_pt.z + sz;
        int x3 = box.max_pt.x;
        int y3 = box.max_pt.y;
        int z3 = box.max_pt.z;

        return std::vector<::box>{
            {{x1, y1, z1}, {x2-1, y2-1, z2-1}},
            {{x2, y1, z1}, {x3, y2 - 1, z2-1}},
            {{x2, y2, z1}, {x3, y3, z2 - 1}},
            {{x1, y2, z1}, {x2 - 1, y3, z2 - 1}},
            {{x1, y1, z2}, {x2 - 1, y2 - 1, z3} },
            {{x2, y1, z2}, {x3, y2 - 1, z3} },
            {{x2, y2, z2}, {x3, y3, z3} },
            {{x1, y2, z2}, {x2 - 1, y3, z3} }
        };
    }

    template<typename T>
    class priority_queue {
        std::multimap<int64_t, T> impl_;
    public:
        priority_queue() {}

        void enqueue(const T& val, int64_t priority) {
            impl_.insert({ priority, val });
        }

        std::tuple<int64_t, T> dequeue() {
            int64_t priority = impl_.rbegin()->first;
            T val = impl_.rbegin()->second;
            impl_.erase(std::prev(impl_.end()));
            return { priority, val };
        }

        bool empty() const {
            return impl_.empty();
        }
    };

    struct nanobot {
        point loc;
        int64_t radius;
    };

    int64_t manhattan_distance(const point& lhs, const point& rhs) {
        return std::abs(lhs.x - rhs.x) +
            std::abs(lhs.y - rhs.y) +
            std::abs(lhs.z - rhs.z);
    }

    bool point_in_box(const point& pt, const ::box& box) {
        return pt.x >= box.min_pt.x && pt.x <= box.max_pt.x &&
            pt.y >= box.min_pt.y && pt.y <= box.max_pt.y &&
            pt.z >= box.min_pt.z && pt.z <= box.max_pt.z;
    }

    int64_t manhattan_distance(const point& pt, const ::box& box) {

        if (point_in_box(pt, box)) {
            return 0;
        }

        constexpr auto inf = std::numeric_limits<int64_t>::max();
        constexpr auto neg_inf = std::numeric_limits<int64_t>::min();

        std::array<::box, 2> z_rays = { {
            {{box.min_pt.x,box.min_pt.y,neg_inf},{box.min_pt.x,box.min_pt.y,box.min_pt.z - 1}},   // below
            {{box.min_pt.x,box.min_pt.y,box.max_pt.z + 1},{box.min_pt.x,box.min_pt.y,inf}}  // above
        } };
        for (const auto& z_ray : z_rays) {
            if (point_in_box(pt, z_ray)) {
                return std::min(std::abs(box.max_pt.z - pt.z), std::abs(box.min_pt.z - pt.z));
            }
        }

        std::array<::box, 2> x_rays = { {
            {{neg_inf,box.min_pt.y,box.min_pt.z},{box.min_pt.x-1,box.max_pt.y,box.max_pt.z}},  // west
            {{ box.max_pt.x+1,box.min_pt.y,box.min_pt.z },{inf, box.max_pt.y, box.max_pt.z}}   // east
        } };
        for (const auto& x_ray : x_rays) {
            if (point_in_box(pt, x_ray)) {
                return std::min(std::abs(box.max_pt.x - pt.x), std::abs(box.min_pt.x - pt.x));
            }
        }

        std::array<::box, 2> y_rays = {{
            {{ box.min_pt.x, neg_inf, box.min_pt.z },{box.max_pt.x, box.min_pt.y-1, box.max_pt.z }}, // south
            {{ box.min_pt.x, box.max_pt.y+1, box.min_pt.z },{box.max_pt.x, inf, box.max_pt.z }}   // north
        }};
        for (const auto& y_ray : y_rays) {
            if (point_in_box(pt, y_ray)) {
                return std::min(std::abs(box.max_pt.y - pt.y), std::abs(box.min_pt.y - pt.y));
            }
        }

        std::array<point, 8> corners = { {
            {box.min_pt.x, box.min_pt.y, box.min_pt.z},
            {box.max_pt.x, box.min_pt.y, box.min_pt.z},
            {box.max_pt.x, box.max_pt.y, box.min_pt.z},
            {box.min_pt.x, box.max_pt.y, box.min_pt.z},
            {box.min_pt.x, box.min_pt.y, box.max_pt.z},
            {box.max_pt.x, box.min_pt.y, box.max_pt.z},
            {box.max_pt.x, box.max_pt.y, box.max_pt.z},
            {box.min_pt.x, box.max_pt.y, box.max_pt.z}
        } };

        return r::min(
            corners | rv::transform(
                [pt](auto&& corner) {
                    return manhattan_distance(pt, corner);
                }
            )
        );
    }

    nanobot string_to_nanobot(const std::string& inp) {
        auto values = aoc::extract_numbers_int64(inp, true);
        return {
            {values[0], values[1], values[2]},
            values[3]
        };
    }

    box bounds(const std::vector<nanobot>& inp) {
        auto x_vals = inp | rv::transform([](auto&& bot) {return bot.loc.x; });
        auto y_vals = inp | rv::transform([](auto&& bot) {return bot.loc.y; });
        auto z_vals = inp | rv::transform([](auto&& bot) {return bot.loc.z; });
        return {
            { r::min(x_vals), r::min(y_vals), r::min(z_vals)},
            { r::max(x_vals), r::max(y_vals), r::max(z_vals)}
        };
    }

    int64_t power_of_two_ceiling(int64_t v) {
        int64_t power_of_two = 1;
        while (power_of_two < v) {
            power_of_two *= 2;
        }
        return power_of_two;
    }

    box initial_box(const std::vector<nanobot>& bots) {
        auto box = bounds(bots);
        std::array<int64_t, 6> dimensions = { {
            std::abs(box.min_pt.x), std::abs(box.min_pt.y), std::abs(box.min_pt.x),
            std::abs(box.max_pt.x), std::abs(box.max_pt.y), std::abs(box.max_pt.x)
        } };
        auto sz = power_of_two_ceiling(r::max(dimensions));
        return { {-sz,-sz,-sz}, {sz-1,sz-1,sz-1} };
    }

    int bots_in_range(const std::vector<nanobot>& bots, const box& box) {
        return r::count_if(
            bots,
            [&](auto&& bot) {
                if (point_in_box(bot.loc, box)) {
                    return true;
                }
                return bot.radius >= manhattan_distance(bot.loc, box);
            }
        );
    }

    int bots_in_range(const std::vector<nanobot>& bots, const point& pt) {
        return r::count_if(
            bots,
            [&](auto&& bot) {
                return bot.radius >= manhattan_distance(bot.loc, pt);
            }
        );
    }

    void update_top_points(
            const std::vector<nanobot>& bots, const box& rgn, 
            std::vector<point>& top_points, int& top_point_count) {
        for (auto x = rgn.min_pt.x; x <= rgn.max_pt.x; ++x) {
            for (auto y = rgn.min_pt.y; y <= rgn.max_pt.y; ++y) {
                for (auto z = rgn.min_pt.z; z <= rgn.max_pt.z; ++z) {
                    point pt{ x,y,z };
                    auto count = bots_in_range(bots, pt);
                    if (count < top_point_count) {
                        return;
                    }
                    if (count > top_point_count) {
                        top_points.clear();
                        top_point_count = count;
                    }
                    top_points.push_back(pt);
                }
            }
        }
    }

    int64_t do_part_2(const std::vector<nanobot>& bots) {
        
        auto universe = initial_box(bots);
        std::vector<point> top_locations;
        int top_bot_count = 0;
        
        priority_queue<box> queue;
        queue.enqueue(universe, 0);

        while (!queue.empty()) {
            auto [bots_count, box] = queue.dequeue();

            if (bots_count < top_bot_count) {
                continue;
            }

            if (box.max_pt.x - box.min_pt.x + 1 == 2) {
                update_top_points(bots, box, top_locations, top_bot_count);
                continue;
            }

            auto subdivisions = subdivide_power_of_two_box(box);
            for (const auto& next_cube : subdivisions) {
                queue.enqueue(next_cube, bots_in_range(bots, next_cube));
            }
        }

        return r::min(
            top_locations | rv::transform(
                [](auto&& pt) {
                    return manhattan_distance(pt, point{ 0,0,0 });
                }
            )
        );
    }

    int do_part_1(const std::vector<nanobot>& inp) {
        auto strongest_nanobot = r::max(
            inp,
            [](auto&& lhs, auto&& rhs) {
                return lhs.radius < rhs.radius;
            }
        );
        return r::count_if(
            inp,
            [&](auto&& nano) {
                auto dist = manhattan_distance(nano.loc, strongest_nanobot.loc);
                return dist <= strongest_nanobot.radius;
            }
        );
    }
}

void aoc::y2018::day_23(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2018, 23)
        ) | rv::transform(
            string_to_nanobot
        ) | r::to<std::vector>();

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
}
