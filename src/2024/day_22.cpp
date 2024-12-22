
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    auto iterate(auto seed, auto iter_fn) {
        return rv::iota(0) | rv::transform(
            [state = std::move(seed), fn = std::move(iter_fn)](auto) mutable {
                auto current = state;
                state = fn(state);
                return current;
            }
        );
    }

    auto nth_item(auto&& range, size_t n) {

        // rv::take(n) / rv::drop(n) don't work
        // because iterate above is defined as transforming
        // an itoa sequence but since it just ignores
        // the argument to transform's lambda, manipulating
        // the transformed sequence doesn't do what we
        // want. Don't know if there is way to do this
        // without caching everything.

        decltype(*range.begin()) v;
        for (auto i : range | rv::take(n)) {
            v = i;
        }
        return v;
    }

    int64_t pseudorandom_step(int64_t v) {
        v = ((v *   64) ^ v) % 16777216;
        v = ((v /   32) ^ v) % 16777216;
        v = ((v * 2048) ^ v) % 16777216;
        return v;
    }

    int64_t do_part_1(const std::vector<int64_t>& secrets) {
        return r::fold_left(
            secrets | rv::transform(
                [](auto secret) {
                    return nth_item(iterate(secret, pseudorandom_step), 2001);
                }
            ),
            0ll,
            std::plus<int64_t>()
        );
    }

    using quad = std::array<int8_t, 4>;

    quad make_quad(auto&& rng) {
        quad q;
        r::copy(rng, q.begin());
        return q;
    }

    struct hash_quad {
        size_t operator()(const quad& q) const {
            size_t seed = 0;
            boost::hash_combine(seed, q[0]);
            boost::hash_combine(seed, q[1]);
            boost::hash_combine(seed, q[2]);
            boost::hash_combine(seed, q[3]);
            return seed;
        }
    };

    template<typename T>
    using quad_map = std::unordered_map<quad, T, hash_quad>;

    quad_map<int64_t> price_per_quad(int64_t seed) {
        auto pseudo_rand_seq = iterate(
                seed, pseudorandom_step
            ) | rv::take(2001) | r::to<std::vector>();
        return pseudo_rand_seq | rv::transform(
            [](auto num)->int8_t {
                return static_cast<int8_t>(num % 10ll);
            }
        ) | rv::adjacent_transform<2>(
            [](auto i, auto j)->std::tuple<int8_t, int8_t> {
                return { j, static_cast<int8_t>(j - i) };
            }
        ) | rv::slide(4) | rv::transform(
            [](auto&& seq)->quad_map<int64_t>::value_type {
                auto quad = make_quad(seq | rv::values | r::to<std::vector>());
                auto price = std::get<0>(seq[3]);
                return { quad, price };
            }
        ) | r::to<quad_map<int64_t>>();
    }

    int64_t do_part_2(const std::vector<int64_t>& secrets) {
        quad_map<int64_t> unified;
        for (auto secret : secrets) {
            auto q_to_e = price_per_quad(secret);
            for (const auto& [q, e] : q_to_e) {
                unified[q] += e;
            }
        }
        return r::max(unified | rv::values);
    }
}

void aoc::y2024::day_22(const std::string& title) {

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2024, 22)
        ) | rv::transform(
            [](auto&& str) {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 22: {} ---", title);
    std::println("  part 1: {}", do_part_1(inp) );
    std::println("  part 2: {}", do_part_2(inp) );
    
}
