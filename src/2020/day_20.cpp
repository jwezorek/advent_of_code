#include "../util.h"
#include "../concat.h"
#include "y2020.h"
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

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    struct vec2 {
        int x;
        int y;
    };

    vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    vec2 operator-(const vec2& lhs, const vec2& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    vec2 operator*(int lhs, const vec2& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    bool operator==(const vec2& lhs, const vec2 rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    using grid = std::vector<std::string>;

    enum direction { north, west, south, east };
    auto directions() {
        return rv::iota(0, 4) | rv::transform(
            [](auto i)->direction {
                return static_cast<direction>(i);
            }
        );
    }

    std::string to_string(direction dir) {
        static const std::array<std::string, 4> strs = {
            "north", "west",  "south", "east"
        };
        return strs[dir];
    }

    struct edge {
        int tile_id;
        std::string label;
        direction dir;
        bool flipped;
    };

    vec2 ortho_direction_vector(const vec2& u, const vec2& v) {
        auto delta_pt = v - u;
        return { sgn(delta_pt.x), sgn(delta_pt.y) };
    }

    class tile {
        int id_;
        grid img_;
        int dim_;

        enum corner {
            ne, nw, sw, se
        };

        std::tuple<corner, corner, corner> dir_to_corner_triple(direction dir, bool flipped) const {
            static const std::array<std::tuple<corner, corner,corner>, 4> dir_to_corners = {{
                    {sw,nw,ne},{se,sw,nw},{ne,se,sw},{nw,ne,se}
                }};
            static const std::array<std::tuple<corner, corner, corner>, 4> dir_to_flipped_corners = { {
                    {se,ne,nw},{sw,se,ne},{nw,sw,se},{ne,nw,sw}
                } };
            return (!flipped) ?
                dir_to_corners[dir] :
                dir_to_flipped_corners[dir];
        }

        vec2 corner_point(corner c) const {
            static const std::array<vec2, 4> corners = { 
                vec2{1,1}, vec2{0,1}, vec2{0,0}, vec2{1,0}
            };
            return (dim_-1) * corners[c];
        }

        std::string str_from_corners(const vec2& c1, const vec2& c2) const {
            auto delta = ortho_direction_vector(c1, c2);
            return rv::iota(0, dim_) | rv::transform(
                [&](int k)->char {
                    return at(c1 + k * delta);
                }
            ) | r::to<std::string>();
        }

        

    public:
        tile(int id, const grid& img) : id_(id), img_(img) {
            if (img.size() != img.front().size()) {
                throw std::runtime_error("tiles must be squares");
            }
            dim_ = static_cast<int>(img.size());
        }

        tile(const std::vector<std::string>& strs) :
            tile(aoc::extract_numbers(strs.front()).front(),
                strs | rv::drop(1) | r::to<grid>()) {
        }

        auto pixel_locations(direction dir, bool flipped) const {
            auto [c1, c2, c3] = dir_to_corner_triple(dir, flipped);
            auto corner = corner_point(c2);
            auto left_to_right = ortho_direction_vector(corner, corner_point(c3));
            auto top_to_bottom = ortho_direction_vector(corner, corner_point(c1));
            return rv::cartesian_product(
                rv::iota(0, dim_),
                rv::iota(0, dim_)
            ) | rv::transform(
                [=](auto&& tup)->vec2 {
                    auto [i, j] = tup;
                    auto pt = corner + i * left_to_right + j * top_to_bottom;
                    return pt;
                }
            );
        }

        tile rotate(direction from_dir, bool from_flipped, direction to_dir) const {
            auto rotated = *this;
            auto from_to = rv::zip(
                pixel_locations(from_dir, from_flipped),
                pixel_locations(to_dir, false)
            );
            for (auto [from_pt, to_pt] : from_to) {
                rotated.at(to_pt) = this->at(from_pt);
            }
            return rotated;
        }

        int id() const {
            return id_;
        }

        char at(int x, int y) const {
            return img_[dim_ - (y + 1)][x];
        }

        char at(const vec2& pt) const {
            return at(pt.x, pt.y);
        }

        char& at(int x, int y) {
            return img_[dim_ - (y + 1)][x];
        }

        char& at(const vec2& pt) {
            return at(pt.x, pt.y);
        }

        std::string edge(direction dir, bool flipped = false) const {
            auto [unused, c1, c2] = dir_to_corner_triple(dir, flipped);
            return str_from_corners(corner_point(c1), corner_point(c2));
        }

        auto edges() const {
            return directions() | rv::transform(
                [this](auto dir)->::edge {
                    return {
                        id_,
                        this->edge(dir, false),
                        dir,
                        false
                    };
                }
            );
        }

        auto flipped_edges() const {
            return directions() | rv::transform(
                [this](auto dir)->::edge {
                    return {
                        id_,
                        this->edge(dir, true),
                        dir,
                        true
                    };
                }
            );
        }

        auto all_edges() const {
            return aoc::concat(edges(), flipped_edges());
        }

        void display() const {
            for (const auto& row : img_) {
                std::println("{}", row);
            }
            std::println("");
        }
    };

    using tile_tbl = std::unordered_map<int, tile>;
    tile_tbl parse_tiles(const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        return groups | rv::transform(
            [](auto&& group)->tile_tbl::value_type {
                tile t(group);
                return { t.id(), std::move(t) };
            }
        ) | r::to<tile_tbl>();
    }

    std::string reverse(const std::string& str) {
        auto reversed = str;
        r::reverse(reversed);
        return reversed;
    }

    bool adjacent_directions(direction d1, direction d2) {
        auto diff = std::abs(static_cast<int>(d1) - static_cast<int>(d2));
        return diff == 1 || diff == 3;
    }

    struct corner_tile {
        int id;
        std::vector<direction> adj;
    };

    std::vector<corner_tile> find_corners(const tile_tbl& tiles) {
        std::unordered_multimap<std::string, edge> edge_map;
        for (auto&& tile : tiles | rv::values) {
            for (auto&& e : tile.all_edges()) {
                edge_map.insert({ e.label, e });
            }
        }
        std::vector<corner_tile> output;
        for (auto&& tile : tiles | rv::values) {
            std::unordered_map<direction, int> adjacent_tile_counts;
            for (auto dir : directions()) {
                adjacent_tile_counts[dir] = edge_map.count(reverse(tile.edge(dir)));
            }
            auto adj = adjacent_tile_counts | rv::filter(
                    [](auto&& v) {
                        auto [key, val] = v;
                        return val > 1;
                    }
                ) | rv::keys | r::to<std::vector<direction>>();
            if (adj.size() == 2 && adjacent_directions(adj.front(), adj.back())) {
                corner_tile ct(tile.id(), adj);
                r::sort(ct.adj);
                output.push_back(ct);
            }
        }
        return output;
    }

    grid build_image(const tile_tbl& tiles) {
        return {};
    }

    int water_roughness(const grid& grid) {
        return 0;
    }
}

void aoc::y2020::day_20(const std::string& title) {
    auto tiles = parse_tiles(
        aoc::file_to_string_vector(aoc::input_path(2020, 20))
    );

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", r::fold_left(
        find_corners(tiles) | rv::transform([](auto ct) {return ct.id;}), 1, std::multiplies<int64_t>())
    );
    auto image = build_image(tiles);
    std::println("  part 2: {}", water_roughness(image));
}