
#include "../util/util.h"
#include "../2015/md5.h"
#include "y2016.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    std::optional<char> first_tripled_char(const std::string& str) {
        auto adjacent_view = str | rv::adjacent<3>;

        auto triplet = r::find_if(
            adjacent_view,
            [](const auto& triplet) {
                auto [a, b, c] = triplet;
                return a == b && b == c;
            }
        );

        // Check if a triplet was found
        if (triplet != r::end(adjacent_view)) {
            return std::get<0>(*triplet);
        }

        return std::nullopt;

    }

    bool contains_quintuple(const std::string& str, char ch) {

        auto adjacent_view = str | rv::adjacent<5>;

        auto quintuple = r::find_if(
            adjacent_view,
            [ch](const auto& quint) {
                auto [a, b, c, d, e] = quint;
                return a == ch && b == ch && c == ch &&
                    d == ch && e == ch;
            }
        );

        return (quintuple != r::end(adjacent_view));
    }

    std::string generate_hash_no_stretching(const std::string& str) {
        static std::unordered_map<std::string, std::string> memos;
        if (!memos.contains(str)) {
            return memos[str] = md5(str);
        }
        return memos.at(str);
    }

    std::string generate_hash(const std::string& str, bool use_stretching) {
        if (!use_stretching) {
            return generate_hash_no_stretching(str);
        }
        static std::unordered_map<std::string, std::string> memos;
        if (memos.contains(str)) {
            return memos.at(str);
        }
        auto hash = generate_hash_no_stretching(str);
        for (int i = 0; i < 2016; ++i) {
            hash = md5(hash);
        }
        memos[str] = hash;
        return hash;
    }

    bool is_key(const std::string& salt, int index, char ch, bool use_stretching) {
        for (int i = index + 1; i <= index + 1000; ++i) {
            auto hash = generate_hash(salt + std::to_string(i), use_stretching);
            if (contains_quintuple(hash, ch)) {
                return true;
            }
        }
        return false;
    }

    int index_of_nth_one_time_pad_key(const std::string& salt, int n, bool use_stretching) {
        int index = 0;
        for (int i = 0; i < n; ++i) {
            bool key = false;
            do {
                auto hash = generate_hash(
                    salt + std::to_string(index),
                    use_stretching
                );
                auto tri = first_tripled_char(hash);
                if (!tri) {
                    ++index;
                    continue;
                }
                key = is_key(salt, index, *tri, use_stretching);
                ++index;
            } while (!key);
        }
        return index-1;
    }
}

void aoc::y2016::day_14(const std::string& title) {

    auto inp = aoc::file_to_string(
            aoc::input_path(2016, 14)
        ); 

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", 
        index_of_nth_one_time_pad_key(inp, 64, false));
    std::println("  part 2: {}",
        index_of_nth_one_time_pad_key(inp, 64, true));
    
}
