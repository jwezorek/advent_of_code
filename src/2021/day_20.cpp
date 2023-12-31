#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <bitset>
#include <numeric>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using image_t = std::vector<std::vector<int>>;

    template<typename T>
    std::vector<std::vector<T>> make_2d_array(int wd, int hgt) {
        return std::vector<std::vector<T>>(hgt, std::vector<T>(wd, T{}));
    }

    template<typename T>
    std::tuple<int, int> get_wd_and_hgt(const std::vector<std::vector<T>>& ary) {
        return {
            ary[0].size(),
            ary.size()
        };
    }

    template<typename T>
    std::vector<std::vector<T>> make_2d_array(const std::vector<std::vector<T>>& g) {
        auto [wd, hgt] = get_wd_and_hgt(g);
        return make_2d_array<T>(wd, hgt);
    }

    std::tuple<std::string, image_t>  parse_day20_input(const std::vector<std::string>& lines) {
        image_t image;
        std::transform(std::next(std::next(lines.begin())), lines.end(), std::back_inserter(image),
            [](const std::string& row_str) {
                std::vector<int> row(row_str.size());
                std::transform(row_str.begin(), row_str.end(), row.begin(),
                    [](auto ch) {return (ch == '#') ? 1 : 0; }
                );
                return row;
            }
        );

        auto dict_str = lines[0];
        std::reverse(dict_str.begin(), dict_str.end());
        std::replace(dict_str.begin(), dict_str.end(), '#', '1');
        std::replace(dict_str.begin(), dict_str.end(), '.', '0');

        return { dict_str, image };
    }

    image_t pad_image(const image_t& image, int padding) {
        if (padding == 0) {
            return image;
        }
        auto [wd, hgt] = get_wd_and_hgt(image);
        image_t padded = make_2d_array<int>(wd + 2 * padding, hgt + 2 * padding);

        auto dst_row_iter = padded.begin() + padding;
        for (const auto& row : image) {
            auto dst_col_iter = dst_row_iter->begin() + padding;
            std::copy(row.begin(), row.end(), dst_col_iter);
            ++dst_row_iter;
        }
        return padded;
    }

    image_t remove_padding(const image_t& image, int padding) {
        if (padding == 0) {
            return image;
        }
        auto [wd, hgt] = get_wd_and_hgt(image);
        image_t depadded = make_2d_array<int>(wd - 2 * padding, hgt - 2 * padding);

        auto dst_row_iter = depadded.begin();
        for (auto src_row_iter = image.begin() + padding; src_row_iter != image.end() - padding; ++src_row_iter) {
            std::copy(src_row_iter->begin() + padding, src_row_iter->end() - padding, dst_row_iter->begin());
            ++dst_row_iter;
        }
        return depadded;
    }

    int get_neighborhood_key(const image_t& g, int x, int y) {
        std::stringstream ss;
        ss << g[y - 1][x - 1] << g[y - 1][x] << g[y - 1][x + 1];
        ss << g[y][x - 1] << g[y][x] << g[y][x + 1];
        ss << g[y + 1][x - 1] << g[y + 1][x] << g[y + 1][x + 1];

        return std::stoi(ss.str(), 0, 2);
    }

    image_t enhance_image(const std::bitset<512>& dictionary, const image_t& image, int padding, int depadding) {
        auto g_src = pad_image(image, padding);

        auto g_dst = make_2d_array<int>(g_src);
        auto [wd, hgt] = get_wd_and_hgt(g_src);
        for (int y = 1; y < hgt - 1; ++y) {
            for (int x = 1; x < wd - 1; ++x) {
                bool pixel_is_on = dictionary.test(get_neighborhood_key(g_src, x, y));
                g_dst[y][x] = (pixel_is_on) ? 1 : 0;
            }
        }

        return remove_padding(g_dst, depadding);
    }

    uint64_t count_on_pixels(const image_t& img) {
        uint64_t count = 0;
        auto [wd, hgt] = get_wd_and_hgt(img);
        for (int y = 0; y < hgt; ++y) {
            for (int x = 0; x < wd; ++x) {
                count += img[y][x];
            }
        }
        return count;
    }

    uint64_t count_pixels_in_range(const image_t& img, int x1, int y1, int x2, int y2) {
        uint64_t count = 0;
        auto [wd, hgt] = get_wd_and_hgt(img);
        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {

                count += img[y][x];
            }
        }
        return count;
    }

    image_t enhance_twice(const std::bitset<512>& dictionary, const image_t& image) {
        return enhance_image(dictionary, enhance_image(dictionary, image, 10, 0), 0, 6);
    }

    uint64_t do_part_1(const std::bitset<512>& dictionary, const image_t& img) {
        return count_on_pixels(enhance_twice(dictionary, img));
    }

    uint64_t do_part_2(const std::bitset<512>& dictionary, const image_t& img) {
        auto enhanced_img = img;
        for (int i = 0; i < 25; ++i) {
            enhanced_img = enhance_twice(dictionary, enhanced_img);
        }
        return count_on_pixels(enhanced_img);
    }
}

void aoc::y2021::day_20(const std::string& title) {
    auto [dictionary_str, image] = parse_day20_input(
        aoc::file_to_string_vector(aoc::input_path(2021, 20))
    );
    std::bitset<512> dictionary(dictionary_str);

    std::println("--- Day 20: {} ---", title);
    std::println("  part 1: {}", do_part_1(dictionary, image));
    std::println("  part 2: {}", do_part_2(dictionary, image));
}