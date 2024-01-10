#include "../util.h"
#include "../concat.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <sstream>

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

    using image = std::vector<std::string>;

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
        image img_;
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
        tile() : id_(-1) {}
        tile(int id, const image& img) : id_(id), img_(img) {
            if (img.size() != img.front().size()) {
                throw std::runtime_error("tiles must be squares");
            }
            dim_ = static_cast<int>(img.size());
        }

        tile(const std::vector<std::string>& strs) :
            tile(aoc::extract_numbers(strs.front()).front(),
                strs | rv::drop(1) | r::to<::image>()) {
        }

        const image& image() const {
            return img_;
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

        int dimension() const {
            return dim_;
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

    using edge_map = std::unordered_multimap<std::string, edge>;
    edge_map build_edge_map(const tile_tbl& tiles) {
        std::unordered_multimap<std::string, edge> edge_map;
        for (auto&& tile : tiles | rv::values) {
            for (auto&& e : tile.all_edges()) {
                edge_map.insert({ e.label, e });
            }
        }
        return edge_map;
    }

    std::vector<corner_tile> find_corners(const tile_tbl& tiles) {
        auto edge_map = build_edge_map(tiles);
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

    tile southwest_corner(const tile_tbl& tiles) {
        auto corners = find_corners(tiles);
        auto iter = r::find_if(corners,
            [](auto&& c) {
                return c.adj[0] == north && c.adj[1] == east;
            }
        );
        if (iter == corners.end()) {
            // both my input and the sample input has
            // a corner correctly oriented to be the SW corner
            // so did not implement rotation here.
            throw std::runtime_error("TODO: handle this");
        }
        return tiles.at(iter->id);
    }

    using tile_grid = std::vector<std::vector<tile>>;

    void display_row(const tile_grid& g, int row) {
        auto grid_sz = static_cast<int>(g.size());
        auto img_sz = g[row][0].dimension();
        for (int i = 0; i < img_sz; i++) {
            std::stringstream ss;
            for (int j = 0; j < grid_sz; j++) {
                ss << g[row][j].image()[i] << " ";
            }
            std::println("{}", ss.str());
        }
    }

    void display_grid(const tile_grid& g) {
        auto grid_sz = static_cast<int>(g.size());
        for (int i = 0; i < grid_sz; ++i) {
            display_row(g, i);
            std::println("");
        }
    }

    void fill_row(tile_grid& grid, int row, const tile& left_tile, const edge_map& edges, const tile_tbl& tiles) {
        int n = static_cast<int>(grid.size());
        grid[row][0] = left_tile;
        for (int col = 1; col < n; ++col) {
            const auto prev = grid[row][col - 1];
            auto rng = edges.equal_range(reverse(prev.edge(east)));
            for (const auto& [_, val] : r::subrange(rng.first, rng.second)) {
                if (val.tile_id != prev.id()) {
                    grid[row][col] = tiles.at(val.tile_id).rotate(val.dir, val.flipped, west);
                    break;
                }
            }
        }
    }

    tile find_tile_above(const tile& tile_below, const edge_map& edges, const tile_tbl& tiles) {
        auto rng = edges.equal_range(reverse(tile_below.edge(north)));
        for (const auto& [_, val] : r::subrange(rng.first, rng.second)) {
            if (val.tile_id != tile_below.id()) {
                return tiles.at(val.tile_id).rotate(val.dir, val.flipped, south);
            }
        }
        throw std::runtime_error("something is wrong");
    }

    image tile_grid_row_to_image(const tile_grid& tiles, int row) {
        auto grid_sz = static_cast<int>(tiles.size());
        auto img_sz = tiles[row][0].dimension();
        image output;
        for (int i = 1; i < img_sz-1; i++) {
            std::stringstream ss;
            for (int j = 0; j < grid_sz; j++) {
                ss << tiles[row][j].image()[i].substr(1, img_sz - 2);
            }
            output.push_back(ss.str());
        }
        return output;
    }

    image tile_grid_to_image(const tile_grid& tiles) {
        image output;
        for (int row = 0; row < tiles.size(); ++row) {
            auto row_image = tile_grid_row_to_image(tiles, row);
            for (auto pixel_row : row_image) {
                output.push_back(pixel_row);
            }
        }
        return output;
    }

    image build_image(const tile_tbl& tiles) {
        auto edges = build_edge_map(tiles);
        int num_tiles = static_cast<int>(tiles.size());
        int grid_dim = static_cast<int>(std::sqrt(num_tiles));
        tile_grid grid(grid_dim, std::vector<tile>(grid_dim));

        auto sw_corner = southwest_corner(tiles);
        fill_row(grid, grid_dim - 1, sw_corner, edges, tiles);
        for (int row = grid_dim - 2; row >= 0; --row) {
            tile left_tile = find_tile_above(grid[row + 1][0], edges, tiles);
            fill_row(grid, row, left_tile, edges, tiles);
        }

        return tile_grid_to_image(grid);
    }

    void display_image(const image& img) {
        for (const auto& row : img) {
            std::println("{}", row);
        }
        std::println("\n");
    }

    int water_roughness(const image& grid) {
        return 0;
    }
}

void aoc::y2020::day_20(const std::string& title) {
    auto tiles = parse_tiles(
        aoc::file_to_string_vector(aoc::input_path(2020, 20, "test"))
    );
    
    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", r::fold_left(
        find_corners(tiles) | rv::transform([](auto ct) {return ct.id;}), 1, std::multiplies<int64_t>())
    );
    auto image = build_image(tiles);
    display_image(image);
    std::println("  part 2: {}", water_roughness(image));
    
}