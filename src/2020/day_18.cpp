#include "../util.h"
#include "y2020.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    class expr {
    public: 
        virtual int64_t eval() const = 0;
    };

    using expr_ptr = std::shared_ptr<expr>;

    class num_expr : public expr {
        int64_t val_;
    public:
        num_expr(int64_t val) : val_(val) {}
        int64_t eval() const override {
            return val_;
        }
    };

    struct term {
        bool is_add;
        expr_ptr val;
    };

    class no_precedence_expr : public expr {
        std::vector<term> terms_;
    public:
        no_precedence_expr(const std::vector<term>& terms) : terms_(terms)
        { }

        int64_t eval() const override {
            int64_t result = 0;
            for (auto term : terms_) {
                auto val = term.val->eval();
                if (term.is_add) {
                    result += val;
                } else {
                    result *= val;
                }
            }
            return result;
        }
    };

    class add_expr : public expr {
        std::vector<expr_ptr> exprs_;
    public:
        add_expr(const std::vector<expr_ptr>& exprs) : exprs_(exprs) {
        }
        int64_t eval() const override {
            return r::fold_left(
                exprs_ | rv::transform([](auto&& e) {return e->eval(); }),
                0ull,
                std::plus<int64_t>()
            );
        }
    };

    class multiply_expr : public expr {
        std::vector<expr_ptr> exprs_;
    public:
        multiply_expr(const std::vector<expr_ptr>& exprs) : exprs_(exprs) {
        }
        int64_t eval() const override {
            return r::fold_left(
                exprs_ | rv::transform([](auto&& e) {return e->eval(); }),
                1ull,
                std::multiplies<int64_t>()
            );
        }
    };

    std::vector<std::string> tokenize(const std::string& inp) {
        std::vector<std::string> tokens;
        auto iter = inp.begin();
        while (iter != inp.end()) {
            if (std::isspace(*iter)) {
                ++iter;
                continue;
            }
            if (!std::isdigit(*iter)) {
                tokens.push_back(std::string(1, *iter++));
                continue;
            }
            auto num_start = iter;
            while (iter != inp.end() && std::isdigit(*iter)) {
                ++iter;
            }
            tokens.push_back(std::string(num_start, iter));
        }
        return tokens;
    }

    using tokens = std::vector<std::string>;
    using iterator = tokens::const_iterator;

    using expr_parser = std::function<expr_ptr(iterator&, const iterator&)>;
    
    expr_ptr parse_number(iterator& i, const iterator& end) {
        if (i == end) {
            return nullptr;
        }
        if (aoc::is_number(*i)) {
            return std::make_shared<num_expr>(std::stoi(*(i++)));
        }
        return nullptr;
    }

    expr_ptr parse_subexpr(iterator& i, const iterator& end, expr_parser parser_fn) {
        if (i == end) {
            return nullptr;
        }
        auto old_i = i;
        if (*i != "(") {
            return nullptr;
        }
        ++i;
        auto expr = parser_fn(i, end);
        if (expr == nullptr) {
            throw std::runtime_error("bad input");
        }
        if (*i != ")") {
            throw std::runtime_error("bad input");
        }
        ++i;
        return expr;
    }

    expr_ptr parse_number_or_subexpr(iterator& i, const iterator& end, expr_parser parser_fn) {
        if (i == end) {
            return nullptr;
        }
        auto num = parse_number(i, end);
        if (num) {
            return num;
        }
        auto subexpr = parse_subexpr(i, end, parser_fn);
        if (subexpr) {
            return subexpr;
        }
        return nullptr;
    }

    std::optional<char> parse_operation(iterator& i, const iterator& end, std::optional<char> op = {}) {
        if (i == end) {
            return {};
        }
        if (*i == "*" || *i == "+") {
            if (op) {
                if (i->front() == op) {
                    return (i++)->front();
                } else {
                    return {};
                }
            }
            return (i++)->front();
        }
        return {};
    }

    expr_ptr parse_no_precedence_expr(iterator& i, const iterator& end, expr_parser parse_fn) {
        auto first = parse_number_or_subexpr(i, end, parse_fn);
        if (!first) {
            return nullptr;
        }
        std::vector<term> terms;
        std::optional<term> next_term = term{ true, first };
        while (next_term) {
            terms.push_back(*next_term);
            auto op = parse_operation(i, end);
            if (!op) {
                next_term = {};
                continue;
            }
            auto arg = parse_number_or_subexpr(i, end, parse_fn);
            if (!arg) {
                next_term = {};
                continue;
            }
            next_term = term{ op == '+', arg };
        }
        return std::make_shared<no_precedence_expr>(terms);
    }

    expr_ptr parse_no_precedence_expr(const std::string& arithmetic) {
        auto tokens = tokenize(arithmetic);
        auto iter = tokens.begin();
        expr_parser parser_fn = [&](iterator& i, const iterator& end) {
            return parse_no_precedence_expr(i, end, parser_fn);
        };
        return parse_no_precedence_expr(iter, tokens.end(), parser_fn);
    }

    expr_ptr parse_add_expr(iterator& i, const iterator& end, expr_parser parse_fn) {
        auto next_term = parse_number_or_subexpr(i, end, parse_fn);
        if (!next_term) {
            return nullptr;
        }
        std::vector<expr_ptr> terms;
        while (next_term) {
            terms.push_back(next_term);
            auto op = parse_operation(i, end, '+');
            if (!op) {
                next_term = {};
                continue;
            }
            next_term = parse_number_or_subexpr(i, end, parse_fn);
        }
        return std::make_shared<add_expr>(terms);
    }

    expr_ptr parse_mult_expr(iterator& i, const iterator& end, expr_parser parse_fn) {
        auto next_factor = parse_add_expr(i, end, parse_fn);
        if (!next_factor) {
            return nullptr;
        }

        std::vector<expr_ptr> factors;
        while (next_factor) {
            factors.push_back(next_factor);
            auto op = parse_operation(i, end, '*');
            if (!op) {
                next_factor = {};
                continue;
            }
            next_factor = parse_add_expr(i, end, parse_fn);
        }

        return std::make_shared<multiply_expr>(factors);
    }

    expr_ptr parse_reversed_precedence_expr(const std::string& arithmetic) {
        auto tokens = tokenize(arithmetic);
        auto iter = tokens.begin();
        expr_parser parser_fn = [&](iterator& i, const iterator& end) {
            return parse_mult_expr(i, end, parser_fn);
        };
        return parse_mult_expr(iter, tokens.end(), parser_fn);
    }
}

void aoc::y2020::day_18(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2020, 18));

    std::println("--- Day 18: {} ---", title);
    std::println("  part 1: {}", 
        r::fold_left(
            input | rv::transform(
                [](auto&& str)->int64_t {
                    return parse_no_precedence_expr(str)->eval();
                }
            ),
            0ll,
            std::plus<int64_t>()
        )
    );
    
    std::println("  part 2: {}",
        r::fold_left(
            input | rv::transform(
                [](auto&& str)->int64_t {
                    return parse_reversed_precedence_expr(str)->eval();
                }
            ),
            0ll,
            std::plus<int64_t>()
        )
    );
}