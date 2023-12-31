#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    class chunk_stack {
    private:
        std::vector<char> stack_;

        char close_for(char open) {
            switch (open) {
            case '(':
                return ')';
            case '[':
                return ']';
            case '{':
                return '}';
            case '<':
                return '>';
            }
            throw std::runtime_error("something is wrong");
        }

    public:
        void open_chunk(char chunk) {
            stack_.push_back(chunk);
        }

        bool close_chunk(char chunk) {
            if (chunk != close_for(stack_.back())) {
                return false;
            }
            else {
                stack_.pop_back();
                return true;
            }
        }

        std::string generate_completion_string() {
            std::string str(stack_.size(), ' ');
            int i = 0;
            for (auto iter = stack_.rbegin(); iter != stack_.rend(); ++iter) {
                str[i++] = close_for(*iter);
            }
            return str;
        }
    };

    bool is_open(char c) {
        return c == '(' || c == '[' || c == '{' || c == '<';
    }

    std::optional<char> parse_to_first_bad_character(const std::string& line) {
        chunk_stack stack;
        for (int i = 0; i < line.size(); ++i) {
            char token = line.at(i);
            if (is_open(token)) {
                stack.open_chunk(token);
            }
            else {
                if (!stack.close_chunk(token)) {
                    return token;
                }
            }
        }
        return std::nullopt;
    }

    std::string parse_and_return_completion_string(const std::string& line) {
        chunk_stack stack;
        for (int i = 0; i < line.size(); ++i) {
            char token = line.at(i);
            if (is_open(token)) {
                stack.open_chunk(token);
            }
            else {
                if (!stack.close_chunk(token)) {
                    return {};
                }
            }
        }
        return stack.generate_completion_string();
    }

    uint64_t do_part_1(const std::vector<std::string>& input) {
        static std::unordered_map<char, uint64_t> chunk_to_score = {
            {')', 3},
            {']', 57},
            {'}', 1197},
            {'>', 25137}
        };

        uint64_t score = 0;
        for (const auto& line : input) {
            auto first_bad_char = parse_to_first_bad_character(line);
            if (first_bad_char) {
                score += chunk_to_score[*first_bad_char];
            }
        }

        return score;
    }

    uint64_t score_completion_string(const std::string& str) {
        static std::unordered_map<char, uint64_t> tok_to_score = {
            {')', 1},
            {']', 2},
            {'}', 3},
            {'>', 4}
        };
        uint64_t score = 0;
        for (int i = 0; i < str.size(); ++i) {
            score = score * 5 + tok_to_score[str[i]];
        }
        return score;
    }

    uint64_t score_completion_strings(const std::vector<std::string>& comp_strings) {
        if (comp_strings.size() % 2 == 0) {
            throw std::runtime_error("even number of incompelte lines");
        }
        std::vector<uint64_t> scores(comp_strings.size());
        std::transform(comp_strings.begin(), comp_strings.end(), scores.begin(),
            score_completion_string
        );
        std::sort(scores.begin(), scores.end());
        return scores[scores.size() / 2];
    }

    uint64_t do_part_2(const std::vector<std::string>& input) {
        std::vector<std::string> completion_strings;
        for (const auto& line : input) {
            auto comp_str = parse_and_return_completion_string(line);
            if (!comp_str.empty()) {
                completion_strings.push_back(comp_str);
            }
        }
        return score_completion_strings(completion_strings);
    }
}

void aoc::y2021::day_10(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 10));

    std::println("--- Day 10: {} ---", title);
    std::println("  part 1: {}", do_part_1(input));
    std::println("  part 2: {}", do_part_2(input));
}