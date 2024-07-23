#pragma once

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <unordered_map>

namespace aoc {

    template<typename T>
    struct vec2 {
        T x;
        T y;

        bool operator==(const vec2<T>& rhs) const {
            return x == rhs.x && y == rhs.y;
        }
    };

    template<typename T>
    vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y
        };
    }

    template<typename T>
    vec2<T> operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y
        };
    }

    template<typename T>
    vec2<T> operator*(T lhs, const vec2<T>& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    template<typename T>
    struct hash_vec2 {
        size_t operator()(const vec2<T>& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    template<typename T>
    using vec2_set = std::unordered_set<vec2<T>, hash_vec2<T>>;

    template<typename T, typename U>
    using vec2_map = std::unordered_map<vec2<T>, U, hash_vec2<T>>;

}