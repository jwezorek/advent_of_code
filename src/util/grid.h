#pragma once

#include <vector>
#include <tuple>

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

        auto begin() const {
            return ary_.begin();
        }

        auto end() const {
            return ary_.end();
        }
    };

}