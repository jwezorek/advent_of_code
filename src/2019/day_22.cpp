#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <boost/multiprecision/cpp_int.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using int128_t = boost::multiprecision::int128_t;

    int128_t mod(int128_t a, int128_t m) {
        int128_t result = a % m;
        if (result < 0) {
            result += m;
        }
        return result;
    }

    std::tuple<int128_t, int128_t, int128_t> extended_euclidean(int128_t a, int128_t b) {
        int128_t x0 = 1, y0 = 0;
        int128_t x1 = 0, y1 = 1;
        while (b != 0) {
            int128_t q = a / b;
            std::tie(a, b) = std::make_tuple(b, a % b);
            std::tie(x0, x1) = std::make_tuple(x1, x0 - q * x1);
            std::tie(y0, y1) = std::make_tuple(y1, y0 - q * y1);
        }
        return { a, x0, y0 }; // a is the gcd, x0 is the coefficient for a, y0 for b
    }

    int128_t multiplicative_inverse_modulo(int128_t a, int128_t m) {
        auto [gcd, x, y] = extended_euclidean(a, m);
        if (gcd != 1) {
            throw std::invalid_argument("a and m must be coprime.");
        }
        return mod(x, m);
    }

    class lcf {
        int128_t a_;
        int128_t b_;
        int128_t m_;
    public:
        lcf(int128_t a, int128_t b, int128_t m) : a_(a), b_(b), m_(m)
        {}

        int128_t a() const {
            return a_;
        }

        int128_t b() const {
            return b_;
        }

        int128_t modulus() const {
            return m_;
        }

        bool operator==(const lcf& other) const {
            return a_ == other.a() && b_ == other.b() && m_ == other.modulus();
        }

        lcf operator()(const lcf& g) const {
            if (m_ != g.modulus()) {
                throw std::invalid_argument("modulus mismatch");
            }

            auto c = g.a();
            auto d = g.b();

            return {
                mod(a_ * c, m_),
                mod(mod(b_ * c, m_) + d, m_),
                m_
            };
        }

        int128_t operator()(int128_t x) const {
            return mod(mod(a_ * x, m_) + b_, m_);
        }
    };

    int128_t invert(const lcf& lcf, int128_t y) {
        auto a_inverse = multiplicative_inverse_modulo(lcf.a(), lcf.modulus());
        return mod(a_inverse * (y - lcf.b()), lcf.modulus());
    }

    enum shuffle_type {
        new_stack,
        cut,
        increment
    };

    struct shuffle {
        shuffle_type type;
        int arg;
    };

    shuffle str_to_shuffle(const std::string& str) {
        auto nums = aoc::extract_numbers(str, true);
        if (str.contains("cut")) {
            return { cut, nums.front() };
        }
        else if (str.contains("increment")) {
            return { increment, nums.front() };
        }
        return { new_stack, 0 };
    }

    lcf shuffle_to_lcf(const shuffle& shuffle, int128_t modulus) {
        auto n = shuffle.arg;
        switch (shuffle.type) {
        case new_stack:
            return { -1, -1, modulus };
        case cut:
            return { 1, -n, modulus };
        case increment:
            return { n, 0, modulus };
        }
        throw std::invalid_argument("unknown shuffle type");
    }

    lcf compose_shuffles(const std::vector<shuffle>& shuffles, int128_t m) {
        return r::fold_left(
            shuffles | rv::transform(
                [m](auto&& s) {
                    return shuffle_to_lcf(s, m);
                }
            ),
            lcf{ 1, 0, m },
            [](const lcf& lhs, const lcf& rhs) {
                return lhs(rhs);
            }
        );
    }

    lcf pow_compose(const lcf& func, int128_t k) {
        auto g = lcf(1, 0, func.modulus());
        auto f = func;
        while (k > 0) {
            if (k % 2 == 1) {
                g = g(f);
            }
            k /= 2;
            f = f(f);
        }
        return g;
    }

    std::string to_string(int128_t n) {
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
}

void aoc::y2019::day_22(const std::string& title) {

    /*
        I couldn't figure part 2 of this one out. This code implements
        what is described here: https://codeforces.com/blog/entry/72593
        doing the composition-by-squaring method for part 2.
    
    */

    auto inp = aoc::file_to_string_vector(
            aoc::input_path(2019, 22)
        ) | rv::transform(
            str_to_shuffle
        ) | r::to<std::vector>();

    std::println("--- Day 1: {} ---", title);
    std::println("  part 1: {}",
        to_string(compose_shuffles(inp, 10007)(2019))
    );

    std::println("  part 2: {}",
        to_string(
            invert(
                pow_compose(
                    compose_shuffles(inp, 119315717514047),
                    101741582076661
                ),
                2020
            )
        )
    );


}