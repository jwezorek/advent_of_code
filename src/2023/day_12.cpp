#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <functional>
#include <numeric>
#include <format>
#include <span>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    struct record {
        std::string row;
        std::vector<int> groups;
    };

    std::string join(const std::vector<int>& vec)
    {
        if (vec.empty()) {
            return {};
        }
        if (vec.size() == 1) {
            return std::to_string(vec.front());
        }
        std::stringstream ss;
        for (int i = 0; i < vec.size()-1; ++i) {
            ss << vec[i] << ", ";
        }
        ss << vec.back();
        return ss.str();
    }

    std::string record_to_string(const record& rec) {
        return std::format(
            "[ {0} : {1} ]", rec.row, join(rec.groups)
        );
    }

    int get_run_length(const std::string& str, int i) {
        char ch = str[i];
        int j;
        for (j = i; j < str.size(); ++j) {
            if (str[j] != ch) {
                break;
            }
        }
        return j - i;
    }

    std::vector<std::string> get_pieces(const std::string& str) {
        int i = 0;
        std::vector<std::string> pieces;
        while (i < str.size()) {
            int run = get_run_length(str, i);
            pieces.emplace_back(run, str[i]);
            i += run;
        }
        return pieces;
    }

    std::string pieces_to_string(const std::vector<std::string>& pieces) {
        std::stringstream ss;
        for (auto piece : pieces) {
            ss << piece;
        }
        return ss.str();
    }

    std::vector<record> parse_input(const std::vector<std::string>& lines) {
        return lines | rv::transform(
                [](const std::string& line)->record {
                    auto parts = aoc::split(line, ' ');
                    return {
                        parts.front(),
                        aoc::extract_numbers(parts.back())
                    };
                }
            ) | r::to<std::vector<record>>();
    }

    bool check_row(const std::string& row, std::vector<int> groups) {
        if (row.find('?') != std::string::npos) {
            throw std::runtime_error("row not assigned");
        }
        auto new_row = row;
        r::replace(new_row, '.', ' ');
        new_row = aoc::collapse_whitespace(new_row);
        auto row_groups = aoc::split(new_row, ' ');
        if (row_groups.size() != groups.size()) {
            return false;
        }
        auto pairs = rv::zip(row_groups, groups);
        return r::find_if(pairs,
                [](const auto& pair)->bool {
                    auto [str, sz] = pair;
                    return str.size() != sz;
                }
            ) == pairs.end();
    }

    std::string assignment(int n, unsigned int index) {
        std::string output(n, '.');
        for (auto i = 0; i < n; ++i) {
            if (index & (1 << i)) {
                output[i] = '#';
            }
        }
        return output;
    }

    int max_index(int n) {
        return (1 << n);
    }

    std::string assign(const std::string& string_with_unknowns, const std::string& assignment) {
        auto output = string_with_unknowns;
        auto iter = assignment.begin();
        for (auto out_iter = output.begin(); out_iter != output.end(); ++out_iter) {
            if (*out_iter == '?') {
                *out_iter = *(iter++);
            }
        }
        return output;
    }

    int number_of_unknowns(const std::string& input) {
        return r::fold_left(
            input | rv::transform(
                [](char ch)->int {
                    return ch == '?' ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    int count_valid_assignments(const record& rec) {
        int num_unknowns = number_of_unknowns(rec.row);
        return r::fold_left(
            rv::iota(0, max_index(num_unknowns)) |
            rv::transform(
                [&](auto index)->int {
                    auto test = assignment(num_unknowns, index);
                    auto filled_in = assign(rec.row, test);
                    return check_row(filled_in, rec.groups) ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

    using rule_fn = std::function<std::optional<record>(const record&)>;

    bool is_dot(char ch) {
        return ch == '.';
    }

    void ltrim_dots(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !is_dot(ch);
            }));
    }

    void rtrim_dots(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !is_dot(ch);
            }).base(), s.end());
    }

    std::string trim_dots(const std::string& str) {
        auto trimmed = str;
        ltrim_dots(trimmed);
        rtrim_dots(trimmed);
        return trimmed;
    }

    std::optional<record> exact_match_unk_bounded(const record& inp) {
        record output;
        auto pieces = get_pieces(inp.row);
        if (pieces.empty() || inp.groups.empty()) {
            return {};
        }
        auto i = pieces.begin();
        int leading_unks = 0;
        if (i->front() == '?') {
            leading_unks = i->size();
            ++i;
        }
        if (i == pieces.end() || i->front() != '#' || i->size() < leading_unks) {
            return {};
        }
        if (i->size() != inp.groups.front()) {
            return {};
        }
        ++i;
        if (i != pieces.end() && i->front() != '?') {
            return {};
        }

        auto new_groups = inp.groups;
        new_groups.erase(new_groups.begin());

        pieces.erase(pieces.begin(), i);
        if (!pieces.empty()) {
            pieces.front().erase(pieces.front().begin());
            if (pieces.front().empty()) {
                pieces.erase(pieces.begin());
            }
        }
        return { {pieces_to_string(pieces), new_groups} };
    }

    std::optional<record> exact_multi_match_dot_bounded(const record& inp) {
        record output;
        auto pieces = get_pieces(inp.row);
        if (pieces.empty() || inp.groups.empty()) {
            return {};
        }
        int len = 0;
        decltype(pieces.begin()) i;
        bool contains_block = false;
        for (i = pieces.begin(); i != pieces.end(); ++i) {
            if (len >= inp.groups.front()) {
                break;
            }
            if (i->front() == '.') {
                break;
            }
            contains_block = contains_block || ( i->front() == '#');
            len += i->size();
        }
        std::string next_piece = (i != pieces.end()) ? *i : "";
        if (contains_block && len == inp.groups.front()  && (next_piece.empty() || next_piece.front() == '.')) {

            pieces.erase(pieces.begin(), i);
            auto groups = inp.groups;
            groups.erase(groups.begin());
            return { {pieces_to_string(pieces), groups} };
        }
        return {};
    }

    record reverse(const record& rec) {
        auto output = rec;
        r::reverse(output.row);
        r::reverse(output.groups);
        return output;
    }

    std::optional<record> apply_rule_backwards(rule_fn rule, const record& rec) {
        auto reversed = reverse(rec);
        return rule(reversed).transform(reverse);
    }

    std::optional<record> apply_rule(rule_fn rule, const record& r) {
        record rec(trim_dots(r.row), r.groups);
        auto forw_results = rule(rec).value_or(rec);
        auto back_results = apply_rule_backwards(rule, forw_results);
        if (back_results) {
            return back_results;
        }
        if (forw_results.row == rec.row) {
            return {};
        } else {
            return forw_results;
        }
    }

    record apply_rules(std::span<rule_fn> rules, const record& r) {
        bool reduced = false;
        auto rec = r;
        do {
            reduced = false;
            for (auto rule : rules) {
                auto result = apply_rule(rule, rec);
                if (result) {
                    rec = *result;
                    reduced = true;
                }
            }
        } while (reduced);
        return rec;
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_12(const std::string& title) {

    auto input = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 12)));

    std::vector<rule_fn> rules = {
        exact_multi_match_dot_bounded,
        exact_match_unk_bounded
    };

    //auto res = apply_rules(rules, test);

    for (auto rec : input) {
        auto new_rec = apply_rules(rules, rec);
        int old_count = count_valid_assignments(rec);
        int new_count = count_valid_assignments(new_rec);
        std::println("{0} => {1}, {2}", record_to_string(rec), record_to_string(new_rec),
            old_count == new_count
        );
        if (old_count != new_count) {
            int old_count = count_valid_assignments(rec);
            int new_count = count_valid_assignments(new_rec);
        }
    }
    
    std::println("--- Day 12: {0} ---\n", title);

    std::println("  part 1: {}",
        r::fold_left(
            input | rv::transform(count_valid_assignments),
            0,
            std::plus<>()
        )
    );

}