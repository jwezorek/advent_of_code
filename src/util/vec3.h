#pragma once

#include <boost/functional/hash.hpp>
#include <unordered_set>
#include <unordered_map>

namespace aoc {

    template<typename T>
    struct vec3 {
        T x;
        T y;
        T z;

        bool operator==(const vec3<T>& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    template<typename T>
    vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
        return {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z,
        };
    }

    template<typename T>
    vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
        return {
            lhs.x - rhs.x,
            lhs.y - rhs.y,
            lhs.z - rhs.z
        };
    }

    template<typename T>
    vec3<T> operator*(T lhs, const vec3<T>& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y,
            lhs * rhs.z
        };
    }

    template<typename T>
    struct hash_vec3 {
        size_t operator()(const vec3<T>& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            boost::hash_combine(seed, pt.z);
            return seed;
        }
    };

    template<typename T>
    using vec3_set = std::unordered_set<vec3<T>, hash_vec3<T>>;

    template<typename T, typename U>
    using vec3_map = std::unordered_map<vec3<T>, U, hash_vec3<T>>;

}