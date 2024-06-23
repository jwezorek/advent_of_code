#include "../util.h"
#include "intcode.h"
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

    using computer = aoc::intcode_computer;

    int count_tractor_beam(const std::vector<int64_t>& program) {
        computer c(program);
        int sum = 0;
        for (int y = 0; y < 50; ++y) {
            for (int x = 0; x < 50; ++x) {
                auto computer = c;
                aoc::input_buffer buffer({ x,y });
                computer.run(buffer);
                sum += computer.output();
            }
        }
        return sum;
    }

    bool test_point(const computer& c, int x, int y) {
        auto computer = c;
        aoc::input_buffer buffer({ x,y });
        computer.run(buffer);
        return computer.output() != 0;
    }

    float upper_slope(const std::vector<int64_t>& program) {
        computer c(program);
        int x = 5;
        int y = 8;
        for (int i = 0; i < 300000; ++i) {
            x++;
            y++;
            while (!test_point(c, x, y)) {
                --x;
            }
        }
        return static_cast<float>(y) / static_cast<float>(x);
    }

    struct point {
        float x;
        float y;
    };

    point find_point(float lower_slope, float upper_slope, float d) {
        auto a = upper_slope;
        auto b = lower_slope;
        auto x = (b * d + d) / (b - a) - d;
        auto y = a * x;
        return { x , y };
    }

    constexpr float k_approx_upper_slope = 1.47405;

    using tractor_beam_fn = std::function<bool(int, int)>;

    int find_right_edge(tractor_beam_fn beam, int y) {
        int x = static_cast<int>(1.47405 + y);
        auto in_beam = beam(x, y);
        int prev;
        if (in_beam) {
            do {
                prev = x;
                ++x;
            } while (beam(x, y));
            return prev;
        } 
        do {
            prev = x;
            --x;
        } while (!beam(x, y));
        return x;
    }

    std::vector<std::string> paint_area(tractor_beam_fn beam, int x, int y, int wd, int hgt) {
        std::vector<std::string> out;
        for (int row = 0; row < hgt; ++row) {
            std::stringstream ss;
            for (int col = 0; col < wd; ++col) { 
                ss << ((beam(x + col, y + row)) ? '#' : '.');
            }
            out.push_back(ss.str());
        }
        return out;
    }

    int find_square_test(tractor_beam_fn beam, int y, int d) {
        int d_minus_one = d - 1;
        int x = find_right_edge(beam, y);
        int left_edge = x - d_minus_one;

        if (!beam(left_edge, y)) {
            return -1;
        }

        int bottom_y = y + d_minus_one;
        if (!beam(left_edge, bottom_y)) {
            return -1;
        }

        if (beam(left_edge, bottom_y + 1)) {
            return 1;
        }

        return 0;
    }

    int find_square(tractor_beam_fn beam, int low_y, int high_y, int d) {
        if (low_y == high_y || high_y == low_y + 1) {
            return high_y;
        }
        int mid = (low_y + high_y) / 2;
        auto test = find_square_test(beam, mid, d);
        if (test < 0) {
            return find_square(beam, mid, high_y, d);
        }
        return find_square(beam, low_y, mid, d);
    }

    std::tuple<int,int> binary_search_for_point(tractor_beam_fn beam, int low_y, int high_y, int d) {
        int y = find_square(beam, low_y, high_y, d);
        int right = find_right_edge(beam, y);
        int x = right - (d - 1);
        return { x,y };
    }

    int do_part_2(const std::vector<int64_t> program) {

        // I think my binary search is not working because
        // the test function has gaps in it ... so just
        // linear search above the row we got via
        // binary search for the correct result...

        auto beam = [&program](int x, int y) {
                computer c(program);
                return test_point(c, x, y);
            };

        auto [x,y] = binary_search_for_point(beam, 100, 2000, 100);
        int min_y;
        for (auto test_row = y; test_row >= y - 20; --test_row) {
            if (find_square_test(beam, test_row, 100) == 0) {
                min_y = test_row;
            }
        }

        int right = find_right_edge(beam, min_y);
        x = right - 99;

        return (x * 10000) + min_y;
    }
}

void aoc::y2019::day_19(const std::string& title) {

    auto program = split(
        aoc::file_to_string(aoc::input_path(2019, 19)), ','
    ) | rv::transform(
        [](auto&& str)->int64_t {
            return aoc::string_to_int64(str);
        }
    ) | r::to<std::vector>();

    std::println("--- Day 19: {} ---", title);

    std::println("  part 1: {} ", count_tractor_beam(program));
    std::println("  part 2: {}",
        do_part_2(program)
    );
}
    