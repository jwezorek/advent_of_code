#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <functional>
#include <numeric>
#include <format>
#include <span>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct record {
        std::string row;
        std::vector<int> groups;
    };

    std::string record_to_string(const record& rec) {
        std::stringstream ss;
        ss << rec.row << " ";
        for (auto v : rec.groups) {
            ss << v << " ";
        }
        return ss.str();
    }

    std::vector<record> parse_input(const std::vector<std::string>& lines) {
        return lines | rv::transform(
            [](const std::string& line)->record {
                auto parts = aoc::split(line, ' ');
                return {
                    parts.front(),
                    aoc::extract_numbers(parts.back())
                };
            }
        ) | r::to<std::vector<record>>();
    }

    bool is_valid_opening(const std::string& row, int i, int block_sz) {
        // no "#' to the left...
        if (row.substr(0, i).contains('#')) {
            return false;
        }
        // the block fits...
        auto block_str = row.substr(i, block_sz);
        if (block_str.contains('.')) {
            return false;
        }
        // the block is properly terminated...
        if ((i + block_sz < row.size() && row[i + block_sz] == '#') || (i + block_sz > row.size())) {
            return false;
        }
        return true;
    }

    record rec_from_opening(const record& rec, int i) {
        auto block_sz = rec.groups.front();
        auto end_of_block = i + block_sz;
        auto new_row = rec.row.substr(end_of_block, rec.row.size() - end_of_block);
        if (!new_row.empty()) {
            // if it hasn't reached the end of the row
            // then there needs to be space for the block
            // terminating '.';
            new_row.erase(new_row.begin());
        }

        auto new_groups = rec.groups;
        new_groups.erase(new_groups.begin());
        return { new_row, new_groups };
    }

    bool is_empty_or_dots(const std::string& str) {
        if (str.empty()) {
            return true;
        }
        return r::all_of(str, [](char ch) {return ch == '.'; });
    }

    int64_t count_valid(const record& rec, std::unordered_map<std::string, int64_t>& memos) {
        if (!rec.row.contains('#') && rec.groups.empty()) {
            return 1;
        }
        if (rec.row.contains('#') && rec.groups.empty()) {
            return 0;
        }
        if (is_empty_or_dots(rec.row) && !rec.groups.empty()) {
            return 0;
        }

        auto key = record_to_string(rec);
        auto iter = memos.find(key);
        if (iter != memos.end()) {
            return iter->second;
        }

        int64_t sum = 0;
        for (int i = 0; i <= rec.row.size(); ++i) {
            if (is_valid_opening(rec.row, i, rec.groups.front())) {
                sum += count_valid(rec_from_opening(rec, i), memos);
            };
        }

        memos.emplace(key, sum);

        return sum;
    }

    std::vector<record> make_part2_input(const std::vector<record>& inp) {
        std::vector<record> output;
        for (const auto& rec : inp) {
            std::stringstream ss;
            std::vector<int> groups;
            for (int i = 0; i < 5; ++i) {
                ss << rec.row;
                if (i != 4) {
                    ss << '?';
                }
                r::copy(rec.groups, std::back_inserter(groups));
            }
            output.emplace_back(ss.str(), groups);
        }
        return output;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_12(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 12)));

    std::unordered_map<std::string, int64_t> memos;

    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform(
                [&memos](auto&& rec) {
                    return count_valid(rec, memos);
                }
            ),
            0,
            std::plus<>()
        )
    );
    
    auto part2_input = make_part2_input(input);
    std::println("  part 2: {}",
        r::fold_left(
            part2_input | rv::transform(
                [&memos](auto&& rec) {
                    return count_valid(rec, memos);
                }
            ),
            0,
            std::plus<>()
        )
    );
}