#include "../util/util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using image_layer = std::vector<std::vector<int>>;
    using space_image = std::vector<image_layer>;

    space_image to_space_image( const std::vector<int>& vals, 
            int cols, int rows) {
        return vals | rv::chunk(cols) | rv::chunk(rows) |
            r::to<std::vector<std::vector<std::vector<int>>>>();
    }

    int num_digit(const image_layer& layer, int digit) {
        return r::fold_left(
            layer | rv::transform(
                    [](const auto& row) {
                        return rv::all(row);
                    }
                ) | rv::join | rv::transform(
                    [digit](int v) {
                        return v == digit ? 1 : 0;
                    }
                ),
            0,
            std::plus<>()
        );
    }

    int verify_image(const space_image& img) {
        const auto& layer_with_fewest_zeros = r::min(
            img, [](const image_layer& lhs, const image_layer& rhs) {
                return num_digit(lhs, 0) < num_digit(rhs, 0);
            }
        );

        return num_digit(layer_with_fewest_zeros, 1) *
            num_digit(layer_with_fewest_zeros, 2);
    }

    int visible_pixel(const space_image& img, int col, int row) {
        constexpr int k_transparent = 2;
        for (const auto& layer : img) {
            if (layer[row][col] != k_transparent) {
                return layer[row][col];
            }
        }
        return k_transparent;
    }

    image_layer decode_image(const space_image& img) {
        int rows = static_cast<int>(img.front().size());
        int cols = static_cast<int>(img.front().front().size());

        image_layer output(rows, std::vector<int>(cols, 2));
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                output[row][col] = visible_pixel(img, col, row);
            }
        }

        return output;
    }
}

void aoc::y2019::day_08(const std::string& title) {

    auto img = to_space_image(
            aoc::file_to_string(
                    aoc::input_path(2019, 8)
                ) | rv::transform(
                    [](auto ch) {return ch - '0'; }
                ) | r::to<std::vector<int>>(), 
            25, 
            6
        );

    std::println("--- Day 8: {} ---\n", title);
    std::println("  part 1: {}",
        verify_image(img)
    );
    std::println("  part 2:\n"
    );
    auto output = decode_image(img);
    for (const auto& row : output) {
        std::print("  ");
        for (const auto& pix : row) {
            std::print("{}", (pix == 1) ? '#' : ' ');
        }
        std::println("");
    }
}