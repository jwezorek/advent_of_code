#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    auto cycle_pattern(const std::vector<int>& pattern) {
        return rv::iota(0) | rv::transform(
            [&](auto i)->int {
                return pattern[i % pattern.size()];
            }
        );
    }

    auto expand_pattern(const std::vector<int>& pattern, int rep) {
        return cycle_pattern(pattern) |
            rv::transform(
                [rep](int v) {
                    return rv::repeat(v) | rv::take(rep);
                }
            ) | rv::join;
    }

    int fft_digit(const std::vector<int>& inp, const std::vector<int> pattern, int i) {
        auto expanded_pattern = expand_pattern(pattern, i + 1) | rv::drop(1);
        auto sum = r::fold_left(
            rv::zip(
                inp, expanded_pattern | rv::take(inp.size())
            ) | rv::transform(
                [](auto pair)->int {
                    auto [lhs, rhs] = pair;
                    return lhs * rhs;
                }
            ),
            0,
            std::plus<>()
        );
        return std::abs(sum) % 10;
    }

    std::vector<int> fft_phase(const std::vector<int>& inp, const std::vector<int> pattern) {
        int n = static_cast<int>(inp.size());
        return rv::iota(0, n) |
            rv::transform(
                [&](auto i)->int {
                    return fft_digit(inp, pattern, i);
                }
            ) | r::to<std::vector>();
    }

    std::vector<int> fft(const std::vector<int>& inp, const std::vector<int> pattern, int n) {
        auto seq = inp;
        for (int i = 0; i < n; ++i) {
            seq = fft_phase(seq, pattern);
        }
        return seq;
    }

    int first_n_digits(const std::vector<int>& inp, int n) {
        return std::stoi(
            inp | rv::take(n) | rv::transform(
                [](int n)->char {
                    return '0' + n;
                }
            ) | r::to<std::string>()
        );
    }

    using seq_generator = std::function<int(int)>;

    std::vector<int> fast_fft(seq_generator gen, int i, int total_seq_sz, int phases) {
        auto seq = rv::iota(i, total_seq_sz) | rv::transform(gen) | r::to<std::vector>();
        for (int i = 0; i < phases; ++i) {
            std::vector<int> next_seq(seq.size());
            int sum = 0;
            for (int i = seq.size() - 1; i >= 0; --i) {
                sum += seq[i];
                next_seq[i] = sum % 10;
            }
            seq = next_seq;
        }
        return seq;
    }
}

void aoc::y2019::day_16(const std::string& title) {
    auto inp_str = aoc::file_to_string(aoc::input_path(2019, 16));

    auto inp = inp_str | rv::filter(
            [](char ch) {return std::isdigit(ch); }
        ) | rv::transform(
            [](char ch)->int { return ch - '0'; }
        ) | r::to<std::vector<int>>();

    std::println("--- Day 1: {} ---", title);

    std::println("  part 1: {}",
        first_n_digits( fft(inp, { 0, 1, 0, -1 }, 100), 8)
    );

    seq_generator gen = [&inp](int i) { return inp[i % inp.size()]; };
    std::println("  part 2: {}",
        first_n_digits(
            fast_fft(gen, first_n_digits(inp,7), inp.size() * 10000, 100),
            8
        )
    );
}