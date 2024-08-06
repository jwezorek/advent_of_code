
#include "../util/util.h"
#include "../util/vec3.h"
#include "../util/vec2.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

#include <boost/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <unordered_map>
#include <unordered_set>
#include <stack>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using vec3 = aoc::vec3<int64_t>;
}

BOOST_GEOMETRY_REGISTER_POINT_3D(::vec3, int, bg::cs::cartesian, x, y, z)

namespace {

    using rtree_key = bg::model::box<vec3>;
    using rtree_value = std::pair<rtree_key, int>;
    using rtree = bgi::rtree<rtree_value, bgi::rstar<8>>;
    using vec2 = aoc::vec2<int64_t>;

    struct box {
        vec3 min_pt;
        vec3 max_pt;

        bool operator==(const box& other) const {
            return min_pt == other.min_pt && max_pt == other.max_pt;
        }
    };

    struct rectangle {
        vec2 min_pt;
        vec2 max_pt;
    };

    box to_box(const rectangle& rect, int min_z, int max_z) {
        return {
            {rect.min_pt.x, rect.min_pt.y, min_z},
            {rect.max_pt.x, rect.max_pt.y, max_z}
        };
    }

    rtree_key box_to_key(const box& b) {
        return {
            b.min_pt,
            b.max_pt 
        };
    }

    box key_to_box(const rtree_key& key) {
        return {
            key.min_corner(),
            key.max_corner() 
        };
    }

    struct nanobot {
        vec3 loc;
        int64_t radius;
    };

    int64_t manhattan_distance(const vec3& lhs, const vec3& rhs) {
        return std::abs(lhs.x - rhs.x) +
            std::abs(lhs.y - rhs.y) +
            std::abs(lhs.z - rhs.z);
    }

    nanobot string_to_nanobot(const std::string& inp) {
        auto values = aoc::extract_numbers_int64(inp, true);
        return {
            {values[0], values[1], values[2]},
            values[3]
        };
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
                return manhattan_distance(nano.loc, strongest_nanobot.loc) <= strongest_nanobot.radius;
            }
        );
    }

    std::optional<box> intersect(const box& lhs, const box& rhs) {
        vec3 min_pt;
        vec3 max_pt;

        min_pt.x = std::max(lhs.min_pt.x, rhs.min_pt.x);
        min_pt.y = std::max(lhs.min_pt.y, rhs.min_pt.y);
        min_pt.z = std::max(lhs.min_pt.z, rhs.min_pt.z);
        max_pt.x = std::min(lhs.max_pt.x, rhs.max_pt.x);
        max_pt.y = std::min(lhs.max_pt.y, rhs.max_pt.y);
        max_pt.z = std::min(lhs.max_pt.z, rhs.max_pt.z);

        if (min_pt.x <= max_pt.x && min_pt.y <= max_pt.y && min_pt.z <= max_pt.z) {
            return box(min_pt, max_pt);
        } else {
            return std::nullopt;
        }
    }

    std::optional<rectangle> intersect(const rectangle& lhs, const rectangle& rhs) {
        vec2 min_pt;
        vec2 max_pt;

        min_pt.x = std::max(lhs.min_pt.x, rhs.min_pt.x);
        min_pt.y = std::max(lhs.min_pt.y, rhs.min_pt.y);
        max_pt.x = std::min(lhs.max_pt.x, rhs.max_pt.x);
        max_pt.y = std::min(lhs.max_pt.y, rhs.max_pt.y);

        if (min_pt.x <= max_pt.x && min_pt.y <= max_pt.y ) {
            return rectangle(min_pt, max_pt);
        } else {
            return std::nullopt;
        }
    }

    std::vector<rectangle> explode_rectangles(const rectangle& lhs, const rectangle& rhs) {
        auto inter = intersect(lhs, rhs);
        if (!inter) {
            return {};
        }
        std::vector<rectangle> pieces;
        pieces.push_back(*inter);
        constexpr auto neg_infinity = std::numeric_limits<int64_t>::min();
        constexpr auto infinity = std::numeric_limits<int64_t>::max();

        std::array<rectangle, 4> masks = { {
            {{neg_infinity,neg_infinity},{infinity,inter->min_pt.y - 1}}, //top
            {{neg_infinity,inter->max_pt.y + 1},{infinity,infinity}}, //bottom
            {{neg_infinity,inter->min_pt.y}, {inter->min_pt.x - 1,inter->max_pt.y}}, //left
            {{inter->max_pt.x + 1,inter->min_pt.y},{infinity, inter->max_pt.y}}, //right
        } };

        for (const auto& mask : masks) {
            auto piece = intersect(mask, lhs);
            if (piece) {
                pieces.push_back(*piece);
            }
            piece = intersect(mask, rhs);
            if (piece) {
                pieces.push_back(*piece);
            }
        }

        return pieces;
    }

    rectangle project_to_2d(const box& b) {
        return { {b.min_pt.x, b.min_pt.y}, {b.max_pt.x, b.max_pt.y} };
    }

    std::vector<box> explode_boxes(const box& lhs, const box& rhs) {

        auto inter = intersect(lhs, rhs);
        if (!inter) {
            return {};
        }
        std::vector<box> pieces;
        pieces.push_back(*inter);
        constexpr auto neg_inf = std::numeric_limits<int64_t>::min();
        constexpr auto inf = std::numeric_limits<int64_t>::max();

        auto explosion_2D = explode_rectangles(project_to_2d(lhs), project_to_2d(rhs));
        for (const auto& rect : explosion_2D | rv::drop(1)) {
            pieces.push_back(to_box(rect, inter->min_pt.z, inter->max_pt.z));
        }

        std::array<box, 2> masks = { {
            {{neg_inf,neg_inf,neg_inf},{inf,inf,inter->min_pt.z-1}},  // below
            {{neg_inf,neg_inf,inter->max_pt.z + 1},{inf,inf,inf}}   // above
        } };

        for (const auto& mask : masks) {
            auto piece = intersect(mask, lhs);
            if (piece) {
                pieces.push_back(*piece);
            }
            piece = intersect(mask, rhs);
            if (piece) {
                pieces.push_back(*piece);
            }
        }

        return pieces;
    }

    void update_density_tree(rtree& tree, const box& next_box, std::queue<box>& queue) {
        std::vector<rtree_value> results;
        tree.query(bgi::intersects(box_to_key(next_box)), std::back_inserter(results));
        if (results.empty()) {
            tree.insert({ box_to_key(next_box) , 1 });
            return;
        }
        for (const auto& collision : results ) {
            auto collision_box = key_to_box(collision.first);
            int density = collision.second;
            auto explosion = explode_boxes(collision_box, next_box);

            tree.remove(collision);

            if (explosion.empty()) {
                throw std::runtime_error("???");
            }
            tree.insert({ box_to_key(explosion.front()) , density + 1 });
            for (const auto& leftover : explosion | rv::drop(1)) {
                queue.push(leftover);
            }
        }
    }

    rtree make_density_tree(const std::vector<box>& boxes) {
        rtree density_tree;
        std::queue<box> queue(boxes.begin(), boxes.end());
        while (!queue.empty()) {
            auto box = queue.front();
            queue.pop();
            update_density_tree(density_tree, box, queue);
        }
        return density_tree;
    }

    box bounds(const nanobot& bot) {
        return {
            {bot.loc.x - bot.radius, bot.loc.y - bot.radius, bot.loc.z - bot.radius},
            {bot.loc.x + bot.radius, bot.loc.y + bot.radius, bot.loc.z + bot.radius}
        };
    }

    struct nested_box {
        box b;
        int depth;
    };

    std::vector<box> intersect_box_with_all(const box& box, const std::vector<::box>& boxes) {
        std::vector<::box> out;
        for (const auto& b : boxes) {
            if (b == box) {
                continue;
            }
            auto inter = intersect(b, box);
            if (inter) {
                out.push_back(*inter);
            }
        }
        return out;
    }
    int64_t do_part_2(const std::vector<nanobot>& bots) {
        auto boxes = bots | rv::transform(bounds) | r::to<std::vector>();
        auto density = make_density_tree(boxes | rv::take(6) | r::to<std::vector>());
        auto test = density | r::to<std::vector>();

        bool has_intersections = false;
        for (auto [a, b] : aoc::two_combinations(test | rv::keys | r::to<std::vector>())) {
            has_intersections = intersect(key_to_box(a), key_to_box(b)).has_value();
        }

        std::println("density complete {} : {} : {}", density.size(), r::max(density | rv::values), has_intersections);
        return -1;
    }
}

void aoc::y2018::day_23(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
        aoc::input_path(2018, 23)
    ) | rv::transform(
        string_to_nanobot
    ) | r::to<std::vector>();

    rtree_key a = { {1,1,0},{4,4,0} };
    rtree_key b = { {4,2,0},{9,3,0} };
    bool test = bg::intersects(a, b);

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
