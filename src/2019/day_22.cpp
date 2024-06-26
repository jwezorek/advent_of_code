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

    /*
    lcf operator+(const lcf& lhs, const lcf& rhs) {
        if (lhs.modulus() != rhs.modulus()) {
            throw std::invalid_argument("modulus mismatch");
        }
        auto m = lhs.modulus();
        auto a = mod(lhs.a() + rhs.a(), m);
        auto b = mod(lhs.b() + rhs.b(), m);

        return { a, b, m };
    }

    lcf operator-(const lcf& lhs, const lcf& rhs) {
        if (lhs.modulus() != rhs.modulus()) {
            throw std::invalid_argument("modulus mismatch");
        }
        auto m = lhs.modulus();
        auto a = mod(lhs.a() - rhs.a(), m);
        auto b = mod(lhs.b() - rhs.b(), m);

        return { a, b, m };
    }
    */

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
        } else if (str.contains("increment")) {
            return { increment, nums.front() };
        }
        return { new_stack, 0 };
    }

    lcf shuffle_to_lcf(const shuffle& shuffle, int128_t modulus) {
        auto n = shuffle.arg;
        switch (shuffle.type) {
            case new_stack:
                return {-1, -1, modulus };
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

    std::string to_string(int128_t n) {
        std::stringstream ss;
        ss << n;
        return ss.str();
    }
}

void aoc::y2019::day_22(const std::string& title) {

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
        0
    );
}