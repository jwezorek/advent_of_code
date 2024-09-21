
#include "../util/util.h"
#include "y2017.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    bool contains_no_duplicates(const std::vector<std::string>& phrase) {
        std::unordered_set<std::string> word_set;
        for (const auto& word : phrase) {
            if (word_set.contains(word)) {
                return false;
            }
            word_set.insert(word);
        }
        return true;
    }

    bool contains_no_anagrams(const std::vector<std::string>& phrase) {
        std::unordered_set<std::string> anagram_set;
        for (const auto& word : phrase) {
            auto canonical_word = word;
            r::sort(canonical_word);
            if (anagram_set.contains(canonical_word)) {
                return false;
            }
            anagram_set.insert(canonical_word);
        }
        return true;
    }
}

void aoc::y2017::day_04(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2017, 4)
        ) | rv::transform(
            aoc::extract_alphabetic
        ) | r::to<std::vector>();

    std::println("--- Day 4: {} ---", title);
    std::println("  part 1: {}", r::count_if(inp, contains_no_duplicates));
    std::println("  part 2: {}", r::count_if(inp, contains_no_anagrams));
    
}
