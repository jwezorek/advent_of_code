
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <format>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using grid = std::vector<std::vector<int>>;

    int power_at_cell(int x, int y, int serial) {
        auto rack_id = x + 10;
        return ((rack_id * y + serial) * rack_id / 100) % 10 - 5;
    }

    grid construct_grid(int serial) {
        auto g = grid(300, std::vector<int>(300, 0));
        for (int y = 1; y <= 300; ++y) {
            for (int x = 1; x <= 300; ++x) {
                g[y - 1][x - 1] = power_at_cell(x, y, serial);
            }
        }
        return g;
    }

    class summed_area_table {

        grid tbl_;

    public:
        summed_area_table(const grid& g) : tbl_(g) {
            int wd = width();
            int hgt = height();
            for (int y = 0; y < hgt; ++y) {
                for (int x = 0; x < wd; ++x) {
                    int above = (y > 0) ? tbl_[y - 1][x] : 0;
                    int left = (x > 0) ? tbl_[y][x - 1] : 0;
                    int above_left = (x > 0 && y > 0) ? tbl_[y - 1][x - 1] : 0;
                    tbl_[y][x] = g[y][x] + above + left - above_left;
                }
            }
        }

        int sum_of_rect(int x1, int y1, int x2, int y2) const {
            auto D = tbl_[y2][x2];
            auto C = (x1 > 0) ? tbl_[y2][x1 - 1] : 0;
            auto B = (y1 > 0) ? tbl_[y1 - 1][x2] : 0;
            auto A = (x1 > 0 && y1 > 0) ? tbl_[y1 - 1][x1 - 1] : 0;
            return D + A - B - C;
        }

        int width() const {
            return static_cast<int>(tbl_.front().size());
        }

        int height() const {
            return static_cast<int>(tbl_.size());
        }
    };

    std::tuple<int, int, int> max_for_window_sz(const summed_area_table& tbl, int sz) {
        auto wd = tbl.width();
        auto hgt = tbl.height();
        int max_sum = std::numeric_limits<int>::min();
        int max_x = 0;
        int max_y = 0;
        for (int y = 0; y <= hgt - sz; ++y) {
            for (int x = 0; x <= wd - sz; ++x) {
                auto sum = tbl.sum_of_rect(x, y, x + sz - 1, y + sz - 1);
                if (sum > max_sum) {
                    max_sum = sum;
                    max_x = x;
                    max_y = y;
                }
            }
        }
        return { max_x+1, max_y+1, max_sum };
    }

    std::tuple<int, int, int> max_across_all_window_sizes(const summed_area_table& tbl) {
        int max_sum = std::numeric_limits<int>::min();
        int max_x = 0;
        int max_y = 0;
        int max_sz = 0;
        for (int sz = 1; sz <= 300; ++sz) {
            auto [x, y, sum] = max_for_window_sz(tbl, sz);
            if (sum > max_sum) {
                max_sum = sum;
                max_x = x;
                max_y = y;
                max_sz = sz;
            }
        }
        return { max_x, max_y, max_sz };
    }
}

void aoc::y2018::day_11(const std::string& title) {

    int serial_number = std::stoi(
        aoc::file_to_string(
            aoc::input_path(2018, 11)
        )
    );

    auto grid = construct_grid(serial_number);
    auto sums_tbl = summed_area_table(grid);

    std::println("--- Day 11: {} ---", title);

    auto [max_3x3_x, max_3x3_y, _] = max_for_window_sz(sums_tbl, 3);
    std::println("  part 1: {},{}", max_3x3_x, max_3x3_y);

    auto [max_x, max_y, max_sz] = max_across_all_window_sizes(sums_tbl);
    std::println("  part 2: {},{},{}", max_x, max_y, max_sz);
    
}
