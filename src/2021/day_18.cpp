#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct pair_num;

    struct one_num {
        uint64_t value;

        one_num(uint64_t v) :
            value(v)
        {}
    };

    using one_num_ptr = std::shared_ptr<one_num>;
    using pair_num_ptr = std::shared_ptr<pair_num>;
    using snail_num = std::variant<one_num_ptr, pair_num_ptr>;

    struct pair_num {
        snail_num left;
        snail_num right;

        pair_num(const snail_num& l, const snail_num& r) :
            left(l), right(r)
        {}
    };

    using iterator = std::string::const_iterator;

    snail_num parse_snail_num(iterator& iter);

    bool parse_char(iterator& iter, char ch) {
        if (*iter != ch) {
            return false;
        }
        ++iter;
        return true;
    }

    pair_num_ptr parse_pair_num(iterator& iter) {
        if (!parse_char(iter, '[')) {
            return nullptr;
        }
        auto left = parse_snail_num(iter);
        if (!parse_char(iter, ',')) {
            throw std::runtime_error("bad pair");
        }
        auto right = parse_snail_num(iter);
        if (!parse_char(iter, ']')) {
            throw std::runtime_error("bad pair");
        }
        return std::make_shared<pair_num>(left, right);
    }

    std::optional<uint64_t> parse_digit(iterator& iter) {
        if (*iter < '0' || *iter > '9') {
            return std::nullopt;
        }
        uint64_t digit = *iter - '0';
        ++iter;
        return digit;
    }

    one_num_ptr parse_number(iterator& iter) {
        std::vector<uint64_t> digits;
        std::optional<uint64_t> maybe_digit;
        while ((maybe_digit = parse_digit(iter)).has_value()) {
            digits.push_back(*maybe_digit);
        }
        if (digits.empty()) {
            return nullptr;
        }
        uint64_t tens_place = 1;
        uint64_t value = 0;
        for (auto i = digits.rbegin(); i != digits.rend(); ++i) {
            value += *i * tens_place;
            tens_place *= 10;
        }
        return std::make_shared<one_num>(value);
    }

    snail_num parse_snail_num(iterator& iter) {
        auto num_pair = parse_pair_num(iter);
        if (num_pair) {
            return num_pair;
        }
        auto one_num = parse_number(iter);
        if (!one_num) {
            throw std::runtime_error("not a number");
        }
        return one_num;
    }

    snail_num string_to_snum(const std::string& str) {
        auto iter = str.begin();
        auto snum = parse_snail_num(iter);
        return snum;
    }

    template<typename T>
    struct snum_visitor {
        std::function<T(one_num_ptr)> one_num_fn;
        std::function<T(pair_num_ptr)> pair_num_fn;

        snum_visitor(const std::function<T(one_num_ptr)>& fn_one, const std::function<T(pair_num_ptr)>& fn_pair) :
            one_num_fn(fn_one), pair_num_fn(fn_pair)
        { }

        T operator()(std::shared_ptr<one_num> p) {
            return one_num_fn(p);
        }

        T operator()(std::shared_ptr<pair_num> p) {
            return pair_num_fn(p);
        }
    };

    std::string snum_to_string(const snail_num& snum) {
        std::stringstream ss;
        std::visit(
            snum_visitor<void>(
                [&ss](one_num_ptr num)->void {
                    ss << num->value;
                },
                [&ss](pair_num_ptr pair)->void {
                    ss << "[" << snum_to_string(pair->left) << "," << snum_to_string(pair->right) << "]";
                }
            ),
            snum
        );
        return ss.str();
    }

    bool is_leaf(const snail_num& snum) {
        return std::holds_alternative<one_num_ptr>(snum);
    }

    bool is_simple_pair(const snail_num& snum) {
        if (!std::holds_alternative<pair_num_ptr>(snum)) {
            return false;
        }
        const auto& pair = std::get<pair_num_ptr>(snum);
        return is_leaf(pair->left) && is_leaf(pair->right);
    }

    snail_num clone_snum(const snail_num& snum) {
        snail_num clone = std::visit(
            snum_visitor<snail_num>(
                [](one_num_ptr p) {
                    return std::make_shared<one_num>(p->value);
                },
                [](pair_num_ptr p) {
                    return std::make_shared<pair_num>(clone_snum(p->left), clone_snum(p->right));
                }
            ),
            snum
        );
        return clone;
    }

    pair_num_ptr find_pair_to_explode(const snail_num& snum, int depth = 0) {
        if (depth >= 4 && is_simple_pair(snum)) {
            return std::get<pair_num_ptr>(snum);
        }
        if (is_leaf(snum)) {
            return nullptr;
        }
        auto pair = std::get<pair_num_ptr>(snum);
        auto pair_to_explode = find_pair_to_explode(pair->left, depth + 1);
        if (pair_to_explode) {
            return pair_to_explode;
        }
        return find_pair_to_explode(pair->right, depth + 1);
    }

    one_num_ptr rightmost_child(snail_num snum) {
        if (is_leaf(snum)) {
            return std::get<one_num_ptr>(snum);
        }
        return rightmost_child(std::get<pair_num_ptr>(snum)->right);
    }

    one_num_ptr leftmost_child(snail_num snum) {
        if (is_leaf(snum)) {
            return std::get<one_num_ptr>(snum);
        }
        return leftmost_child(std::get<pair_num_ptr>(snum)->left);
    }

    uint64_t get_leaf_value(snail_num snum) {
        if (!is_leaf(snum)) {
            throw std::runtime_error("attempted to get leaf value of non-leaf");
        }
        return std::get<one_num_ptr>(snum)->value;
    }

    bool operator==(const snail_num& a, const snail_num& b) {
        void* ptr_a = std::visit([](const auto& sp)->void* {return sp.get(); }, a);
        void* ptr_b = std::visit([](const auto& sp)->void* {return sp.get(); }, b);
        return ptr_a == ptr_b;
    }

    struct explode_result {
        uint64_t left;
        uint64_t right;
        bool delete_me;
    };

    bool explode(snail_num& snum) {
        std::function<std::optional<explode_result>(snail_num& snum, int depth)> explode_aux;
        explode_aux = [&explode_aux](snail_num& snum, int depth)->std::optional<explode_result> {
            return std::visit(
                snum_visitor<std::optional<explode_result>>(
                    [](one_num_ptr)->std::optional<explode_result> { return std::nullopt; },
                    [depth, &explode_aux](pair_num_ptr p)->std::optional<explode_result> {
                        if (depth >= 4 && is_simple_pair(p)) {
                            return { { get_leaf_value(p->left), get_leaf_value(p->right), true } };
                        }
                        auto left_result = explode_aux(p->left, depth + 1);
                        if (left_result) {
                            if (left_result->delete_me) {
                                p->left = std::make_shared<one_num>(0);
                                left_result->delete_me = false;
                            }
                            if (left_result->right > 0) {
                                one_num_ptr right_neighbor = leftmost_child(p->right);
                                right_neighbor->value += left_result->right;
                                left_result->right = 0;
                            }
                            return left_result;
                        }
                        auto right_result = explode_aux(p->right, depth + 1);
                        if (right_result) {
                            if (right_result->delete_me) {
                                p->right = std::make_shared<one_num>(0);
                                right_result->delete_me = false;
                            }
                            if (right_result->left > 0) {
                                one_num_ptr left_neighbor = rightmost_child(p->left);
                                left_neighbor->value += right_result->left;
                                right_result->left = 0;
                            }
                            return right_result;
                        }
                        return std::nullopt;
                    }
                ),
                snum
            );
            };
        return explode_aux(snum, 0).has_value();
    }

    snail_num make_simple_pair(pair_num* parent, uint64_t left, uint64_t right) {
        one_num_ptr left_ptr = std::make_shared<one_num>(left);
        one_num_ptr right_ptr = std::make_shared<one_num>(right);
        pair_num_ptr pair_ptr = std::make_shared<pair_num>(left_ptr, right_ptr);
        snail_num snum(pair_ptr);
        return snum;
    }

    bool split(snail_num& snum) {
        return std::visit(
            snum_visitor<bool>(
                [](one_num_ptr num)->bool {
                    return false;
                },
                [](pair_num_ptr p)->bool {
                    if (snum_to_string(p) == "[[9,10],20]") {
                        int aaa;
                        aaa = 5;
                    }
                    if (is_leaf(p->left) && get_leaf_value(p->left) >= 10) {
                        auto value = get_leaf_value(p->left);
                        uint64_t left_value = value / 2;
                        uint64_t right_value = value - left_value;
                        p->left = make_simple_pair(nullptr, left_value, right_value);
                        return true;
                    }
                    else {
                        if (split(p->left)) {
                            return true;
                        }
                    }
                    if (is_leaf(p->right) && get_leaf_value(p->right) >= 10) {
                        auto value = get_leaf_value(p->right);
                        uint64_t left_value = value / 2;
                        uint64_t right_value = value - left_value;
                        p->right = make_simple_pair(nullptr, left_value, right_value);
                        return true;
                    }
                    return split(p->right);
                }
            ),
            snum
        );
    }

    void reduce(snail_num& snum) {
        bool exploded = false;
        bool splitted = false;

        do {
            exploded = explode(snum);
            if (exploded) {
                continue;
            }
            splitted = split(snum);

        } while (exploded || splitted);
    }

    snail_num join(const snail_num& snum1, const snail_num& snum2) {
        snail_num joined = std::make_shared<pair_num>(clone_snum(snum1), clone_snum(snum2));
        return joined;
    }

    snail_num add(const snail_num& snum1, const snail_num& snum2) {
        snail_num sum = join(snum1, snum2);
        reduce(sum);
        return sum;
    }

    std::vector<snail_num> to_snums(const std::vector<std::string>& strings) {
        std::vector<snail_num> snums(strings.size());
        std::transform(strings.begin(), strings.end(), snums.begin(), string_to_snum);
        return snums;
    }

    snail_num sum_of_snums(const std::vector<snail_num>& snums) {
        if (snums.size() == 1) {
            return snums[0];
        }
        snail_num sum = snums.front();
        for (int i = 1; i < snums.size(); ++i) {
            sum = add(sum, snums[i]);
        }
        return sum;
    }

    uint64_t magnitude(const snail_num& snum) {
        return std::visit(
            snum_visitor<uint64_t>(
                [](one_num_ptr p) {return p->value; },
                [](pair_num_ptr p) {return 3 * magnitude(p->left) + 2 * magnitude(p->right); }
            ),
            snum
        );
    }

    uint64_t find_largest_sum(const std::vector<snail_num>& snums) {
        static auto find_largest_with_ith = [&snums](int i) {
            uint64_t largest = 0;
            for (int j = 0; j < snums.size(); ++j) {
                if (i == j) {
                    continue;
                }
                auto sum = magnitude(add(snums[i], snums[j]));
                largest = (sum > largest) ? sum : largest;
            }
            return largest;
            };
        uint64_t largest = 0;
        for (int i = 0; i < snums.size(); ++i) {
            auto max_sum_with_ith_item = find_largest_with_ith(i);
            largest = (max_sum_with_ith_item > largest) ? max_sum_with_ith_item : largest;
        }
        return largest;
    }
}

void aoc::y2021::day_18(const std::string& title) {
    auto snums = to_snums(aoc::file_to_string_vector(aoc::input_path(2021, 18)));

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}", magnitude(sum_of_snums(snums)));
    std::println("  part 2: {}", find_largest_sum(snums));
}