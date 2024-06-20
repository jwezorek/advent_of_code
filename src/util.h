#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <ranges>

namespace aoc {

    std::string input_path(int year, int day, const std::string& tag = {});
    std::vector<std::string> file_to_string_vector(const std::string& filename);
    std::string file_to_string(const std::string& filename);
    std::string collapse_whitespace(const std::string& str);
    bool is_number(const std::string& s);
    std::vector<std::string> split(const std::string& s, char delim);
    std::vector<std::vector<int>> strings_to_2D_array_of_digits(const std::vector<std::string>& lines);
    std::vector<int> extract_numbers(const std::string& str, bool allow_negatives = false);
    std::vector<int64_t> extract_numbers_int64(const std::string& str, bool allow_negatives = false);
    std::vector<std::string> extract_alphabetic(const std::string& str);
    std::vector<std::string> extract_alphanumeric(const std::string& str);
    std::string remove_nonalphabetic(const std::string& str);
    std::string remove_nonnumeric(const std::string& str);
    std::string trim(const std::string& str);
    int64_t string_to_int64(const std::string& str);
    std::vector<std::vector<std::string>> group_strings_separated_by_blank_lines(const std::vector<std::string>& lines);

    template <std::size_t... Is>
    auto create_tuple_impl(std::index_sequence<Is...>, const std::vector<std::string>& arguments) {
        return std::make_tuple(arguments[Is]...);
    }

    template <std::size_t N>
    auto create_tuple(const std::vector<std::string>& arguments) {
        return create_tuple_impl(std::make_index_sequence<N>{}, arguments);
    }

    template <std::size_t N>
    auto split_to_tuple(const std::string& s, char delim) {
        auto vec = split(s, delim);
        return create_tuple<N>(vec);
    }
 
    template<typename V, typename H = std::hash< V>>
    class priority_queue {
        using map_impl = std::multimap<int, V>;
        map_impl priority_to_item_;
        std::unordered_map<V, typename map_impl::iterator, H> item_to_mmap_iter_;

    public:
        priority_queue()
        {}

        V extract_min() {
            auto first = priority_to_item_.begin();
            auto loc = first->second;
            priority_to_item_.erase(first);
            item_to_mmap_iter_.erase(loc);
            return loc;
        }

        void insert(const V& loc, int priority) {
            auto iter = priority_to_item_.insert({ priority, loc });
            item_to_mmap_iter_[loc] = iter;
        }

        bool contains(const V& v) const {
            return item_to_mmap_iter_.contains(v);
        }

        void change_priority(const V& loc, int priority) {
            auto iter = item_to_mmap_iter_[loc];
            priority_to_item_.erase(iter);
            item_to_mmap_iter_.erase(loc);
            insert(loc, priority);
        }

        bool empty() const {
            return priority_to_item_.empty(); 
        }

        size_t size() const {
            return priority_to_item_.size();
        }
    };

    auto two_combinations(const auto& container) {
        namespace r = std::ranges;
        namespace rv = std::ranges::views;
        auto n = static_cast<int>(container.size());
        return rv::iota(0,n-1) |
            rv::transform(
                [n](auto i) {
                    return rv::iota(i + 1, n) | rv::transform(
                        [i](auto j) {
                            return std::tuple<int, int>(i, j);
                        }
                    );
                }
            ) | rv::join | rv::transform(
                [&container](auto&& tup) {
                    auto [i, j] = tup;
                    return std::tuple(container.at(i), container.at(j));
                }
            );
    }

    auto two_combos_indices(int n) {
        namespace r = std::ranges;
        namespace rv = std::ranges::views;
        return rv::iota(0, n - 1) |
            rv::transform(
                [n](auto i) {
                    return rv::iota(i + 1, n) | rv::transform(
                        [i](auto j) {
                            return std::tuple<int, int>(i, j);
                        }
                    );
                }
            ) | rv::join;
    }
}