#include "../util/util.h"
#include "y2021.h"
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
    using rules_tbl_t = std::unordered_map<std::string, char>;

    std::tuple<std::string, rules_tbl_t> parse_day_14_input(const std::vector<std::string>& input) {
        std::vector<std::tuple<std::string, char>> rule_pairs(input.size() - 2);
        std::transform(input.begin() + 2, input.end(), rule_pairs.begin(),
            [](const std::string& str)->std::tuple<std::string, char> {
                std::string key(2, ' ');
                key[0] = str[0];
                key[1] = str[1];
                return { key, str[6] };
            }
        );

        rules_tbl_t rules;
        rules.reserve(rule_pairs.size());
        for (const auto& [from, to] : rule_pairs) {
            rules[from] = to;
        }

        return { input[0], std::move(rules) };
    }

    using letter_freqencies_t = std::unordered_map<char, uint64_t>;
    std::tuple<uint64_t, uint64_t> get_min_and_max_freqs(const letter_freqencies_t& freqs) {
        std::vector<uint64_t> values(freqs.size());
        std::transform(freqs.begin(), freqs.end(), values.begin(),
            [](const auto& p) {
                auto [ch, count] = p;
                return count;
            }
        );
        auto [iter_min, iter_max] = std::minmax_element(values.begin(), values.end());
        return { *iter_min, *iter_max };
    }

    using pair_freqencies_t = std::unordered_map<std::string, uint64_t>;

    pair_freqencies_t make_pair_freqencies(const rules_tbl_t& rules) {
        pair_freqencies_t freq;
        freq.reserve(rules.size());
        for (const auto& [key, val] : rules) {
            freq[key] = 0;
        }
        return freq;
    }

    pair_freqencies_t string_to_pair_freqencies(const rules_tbl_t& rules, const std::string& str) {
        pair_freqencies_t freqs = make_pair_freqencies(rules);
        for (uint64_t i = 0; i < str.size() - 1; ++i) {
            auto pair = str.substr(i, 2);
            freqs[pair]++;
        }
        return freqs;
    }

    std::tuple<std::string, std::string> spawn_pairs(const std::string& pair, char  ch) {
        std::string pair_1(2, 0);
        pair_1[0] = pair[0];
        pair_1[1] = ch;

        std::string pair_2(2, 0);
        pair_2[0] = ch;
        pair_2[1] = pair[1];

        return { pair_1, pair_2 };
    }

    pair_freqencies_t perform_one_generation(const rules_tbl_t& rules, const pair_freqencies_t& gen_0) {
        pair_freqencies_t gen_1 = make_pair_freqencies(rules);
        for (auto [pair, count] : gen_0) {
            auto [pair_1, pair_2] = spawn_pairs(pair, rules.at(pair));
            gen_1[pair_1] += count;
            gen_1[pair_2] += count;
        }
        return gen_1;
    }

    pair_freqencies_t perform_n_generations(const rules_tbl_t& rules, const pair_freqencies_t& gen_0, int n) {
        auto freq = gen_0;
        for (int i = 0; i < n; ++i) {
            freq = perform_one_generation(rules, freq);
        }
        return freq;
    }

    std::vector<char> pair_freq_to_unique_letters(const pair_freqencies_t& pairs) {
        std::unordered_set<char> chars;
        for (const auto& [pair, count] : pairs) {
            chars.insert(pair[0]);
            chars.insert(pair[1]);
        }
        std::vector<char> output(chars.size());
        std::copy(chars.begin(), chars.end(), output.begin());
        return output;
    }

    letter_freqencies_t pair_freq_to_letter_freq(const pair_freqencies_t& pairs, char first_char) {

        letter_freqencies_t freq;

        for (auto [pair, count] : pairs) {
            auto ch = pair[1];
            auto iter = freq.find(ch);
            if (iter == freq.end()) {
                freq[ch] = count;
            }
            else {
                iter->second += count;
            }
        }
        freq[first_char]++;

        return freq;
    }

    uint64_t do_day_14_part(const std::string& seed, const rules_tbl_t& rules, int n) {
        auto freq = pair_freq_to_letter_freq(
            perform_n_generations(rules, string_to_pair_freqencies(rules, seed), n),
            seed[0]
        );
        auto [min, max] = get_min_and_max_freqs(freq);
        return max - min;
    }
}

void aoc::y2021::day_14(const std::string& title) {
    auto [seed, rules] = parse_day_14_input(
        aoc::file_to_string_vector(aoc::input_path(2021, 14))
    );

    std::println("--- Day 14: {} ---", title);
    std::println("  part 1: {}", do_day_14_part(seed, rules, 10));
    std::println("  part 2: {}", do_day_14_part(seed, rules, 40));
}