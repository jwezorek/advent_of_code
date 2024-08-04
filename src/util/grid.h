#pragma once

#include <vector>
#include <tuple>
#include <functional>
#include <ranges>
#include "vec2.h"

namespace aoc {

    template<typename T>
    class grid {
        std::vector<T> ary_;
        int cols_;

        size_t index(int col, int row) const {
            return row * cols_ + col;
        }

    public:
        grid(int cols, int rows) : cols_(cols), ary_(cols * rows) {
        }

        grid(int cols, int rows, T val) : cols_(cols), ary_(cols * rows, val) {
        }

        grid(std::tuple<int, int> dim) : grid( std::get<0>(dim), std::get<1>(dim) ) {
        }

        int columns() const {
            return cols_;
        }

        int rows() const {
            return static_cast<int>(ary_.size()) / cols_;
        }

        std::tuple<int, int> dimensions() const {
            return { columns(), rows() };
        }

        //TODO: use operator[] when VS supports it...
        T operator()(int col, int row) const {
            return ary_.at(index(col, row));
        }

        T& operator()(int col, int row) {
            return ary_[index(col, row)];
        }

        T operator()(const vec2<int>& loc) const {
            return ary_.at(index(loc.x, loc.y));
        }

        T& operator()(const vec2<int>& loc) {
            return ary_[index(loc.x, loc.y)];
        }

        auto begin() const {
            return ary_.begin();
        }

        auto end() const {
            return ary_.end();
        }

        template<typename U>
        grid<U> transform(const std::function<U(const T&)>& fn) const {
            auto [wd, hgt] = dimensions();
            grid<U> transformed(wd,hgt);
            for (int y = 0; y < hgt; ++y) {
                for (int x = 0; x < wd; ++x) {
                    transformed(x, y) = fn( (*this)(x, y) );
                }
            }
            return transformed;
        }

        auto region(int ox, int oy, int wd, int hgt) const {
            namespace r = std::ranges;
            namespace rv = std::ranges::views;
            return rv::cartesian_product(
                rv::iota(ox, ox + wd),
                rv::iota(oy, oy + hgt)
            ) | rv::transform(
                [&](auto tup) {
                    auto [x, y] = tup;
                    return (*this)(x, y);
                }
            );
        }

        auto values() const {
            auto [wd, hgt] = dimensions();
            return region(0, 0, wd, hgt);
        }
    };

}