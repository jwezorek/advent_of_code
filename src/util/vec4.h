#pragma once

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <unordered_map>

namespace aoc {

    template<typename T>
    struct vec4 {
        T w;
        T x;
        T y;
        T z;

        bool operator==(const vec4<T>& rhs) const {
            return w == rhs.w && x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    template<typename T>
    vec4<T> operator+(const vec4<T>& lhs, const vec4<T>& rhs) {
        return {
            lhs.w + rhs.w,
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
        };
    }

    template<typename T>
    vec4<T> operator-(const vec4<T>& lhs, const vec4<T>& rhs) {
        return {
            lhs.w - rhs.w,
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    template<typename T>
    vec4<T> operator*(T lhs, const vec4<T>& rhs) {
        return {
            lhs * rhs.w,
            lhs * rhs.x,
            lhs * rhs.y,
            lhs * rhs.z
        };
    }

    template<typename T>
    struct hash_vec4 {
        size_t operator()(const vec4<T>& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.w);
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            boost::hash_combine(seed, pt.z);
            return seed;
        }
    };

    template<typename T>
    using vec4_set = std::unordered_set<vec4<T>, hash_vec4<T>>;

    template<typename T, typename U>
    using vec4_map = std::unordered_map<vec4<T>, U, hash_vec4<T>>;

}