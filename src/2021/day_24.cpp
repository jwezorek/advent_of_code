#include "../util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <variant>
#include <format>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    enum op_code {
        inp,
        add,
        mul,
        div,
        mod,
        eql
    };

    struct command {
        op_code op;
        char var;
        std::optional<std::variant<char, int>> arg;
    };

    op_code str_to_op_code(const std::string& str) {
        static const std::unordered_map<std::string, op_code> tbl = {
            {"inp" , inp},
            {"add" , add},
            {"mul" , mul},
            {"mod" , mod},
            {"div" , div},
            {"eql" , eql}
        };
        return tbl.at(str);
    }

    bool is_variable(const std::string& str) {
        return str == "w" || str == "x" || str == "y" || str == "z";
    }

    std::variant<char, int> str_to_arg(const std::string& str) {
        if (is_variable(str)) {
            return { str.front() };
        }
        return { std::stoi(str) };
    }

    command str_to_command(const std::string& line) {
        auto parts = aoc::split(line, ' ');
        if (parts.size() == 2) {
            return {
                str_to_op_code(parts[0]),
                parts[1].front(),
                {}
            };
        }
        return {
            str_to_op_code(parts[0]),
            parts[1].front(),
            str_to_arg(parts[2])
        };
    }

    using input_t = std::array<int, 14>;

    class expr {
    public:
        virtual std::string to_string() const = 0;
        virtual std::shared_ptr<expr> clone() const = 0;
        virtual std::shared_ptr<expr> simplify() const {
            return clone();
        }
        virtual int eval(const input_t& inp) const = 0;
    };

    using expr_ptr = std::shared_ptr<expr>;

    class num_expr : public expr {
        char val_;
    public:
        num_expr(int v) : val_(v) {}
        std::string to_string() const override {
            return std::to_string(val_);
        };

        expr_ptr clone() const override {
            return std::make_shared<num_expr>(val_);
        }
        int value() const {
            return val_;
        }

        int eval(const input_t& inp) const override {
            return val_;
        }
    };

    class inp_expr : public expr {
        int index_;
    public:
        inp_expr(int index) : index_(index) {}
        std::string to_string() const override {
            return std::format("inp[{}]", index_);
        };

        virtual void find_input(input_t& inp, int target) const {
            inp[index_] = target;
        }

        expr_ptr clone() const override {
            return std::make_shared<inp_expr>(index_);
        }

        int eval(const input_t& inp) const override {
            return inp[index_];
        }
    };

    std::optional<int> get_number(expr_ptr expr) {
        auto ptr = dynamic_cast<num_expr*>(expr.get());
        if (ptr) {
            return ptr->value();
        }
        return {};
    }

    class add_expr : public expr {
        expr_ptr lhs_;
        expr_ptr rhs_;
    public:
        add_expr(expr_ptr lhs, expr_ptr rhs) :
            lhs_(lhs), rhs_(rhs)
        {}

        std::string to_string() const override {
            return std::format("( {} + {} )",
                lhs_->to_string(), rhs_->to_string()
            );
        }

        expr_ptr clone() const override {
            return std::make_shared<add_expr>(lhs_->clone(), rhs_->clone());
        }

        expr_ptr simplify() const override {
            auto lhs = lhs_->simplify();
            auto rhs = rhs_->simplify();

            auto a = get_number(lhs);
            auto b = get_number(rhs);
            if (a && b) {
                return std::make_shared<num_expr>(*a + *b);
            }
            if (a && *a == 0) {
                return rhs;
            }
            if (b && *b == 0) {
                return lhs;
            }

            return std::make_shared<add_expr>(lhs, rhs);
        }

        int eval(const input_t& inp) const override {
            return lhs_->eval(inp) + rhs_->eval(inp);
        }
    };

    class mul_expr : public expr {
        expr_ptr lhs_;
        expr_ptr rhs_;
    public:
        mul_expr(expr_ptr lhs, expr_ptr rhs) :
            lhs_(lhs), rhs_(rhs)
        {}

        std::string to_string() const override {
            return std::format("( {} * {} )",
                lhs_->to_string(), rhs_->to_string()
            );
        }

        expr_ptr clone() const override {
            return std::make_shared<mul_expr>(lhs_->clone(), rhs_->clone());
        }

        expr_ptr simplify() const override {
            auto lhs = lhs_->simplify();
            auto rhs = rhs_->simplify();

            auto a = get_number(lhs);
            auto b = get_number(rhs);
            if (a && b) {
                return std::make_shared<num_expr>(*a * *b);
            }
            if (a && *a == 0) {
                return std::make_shared<num_expr>(0);
            }
            if (b && *b == 0) {
                return std::make_shared<num_expr>(0);
            }
            if (a && *a == 1) {
                return rhs;
            }
            if (b && *b == 1) {
                return lhs;
            }

            return std::make_shared<mul_expr>(lhs, rhs);
        }

        int eval(const input_t& inp) const override {
            return lhs_->eval(inp) * rhs_->eval(inp);
        }
    };

    class div_expr : public expr {
        expr_ptr lhs_;
        expr_ptr rhs_;
    public:
        div_expr(expr_ptr lhs, expr_ptr rhs) :
            lhs_(lhs), rhs_(rhs)
        {
        }

        std::string to_string() const override {
            return std::format("( {} / {} )",
                lhs_->to_string(), rhs_->to_string()
            );
        }

        expr_ptr clone() const override {
            return std::make_shared<div_expr>(lhs_->clone(), rhs_->clone());
        }

        expr_ptr simplify() const override {
            auto lhs = lhs_->simplify();
            auto rhs = rhs_->simplify();

            auto a = get_number(lhs);
            auto b = get_number(rhs);
            if (a && b) {
                return std::make_shared<num_expr>(*a / *b);
            }

            if (a && *a == 0) {
                return std::make_shared<num_expr>(0);
            }

            if (b && *b == 1) {
                return lhs;
            }
 
            return std::make_shared<div_expr>(lhs, rhs);
        }

        int eval(const input_t& inp) const override {
            return lhs_->eval(inp) / rhs_->eval(inp);
        }
    };

    class mod_expr : public expr {
        expr_ptr lhs_;
        expr_ptr rhs_;
    public:
        mod_expr(expr_ptr lhs, expr_ptr rhs) :
            lhs_(lhs), rhs_(rhs)
        {}

        std::string to_string() const override {
            return std::format("( {} % {} )",
                lhs_->to_string(), rhs_->to_string()
            );
        }

        expr_ptr clone() const override {
            return std::make_shared<mod_expr>(lhs_->clone(), rhs_->clone());
        }

        expr_ptr simplify() const override {
            auto lhs = lhs_->simplify();
            auto rhs = rhs_->simplify();

            auto a = get_number(lhs);
            auto b = get_number(rhs);
            if (a && b) {
                return std::make_shared<num_expr>(*a % *b);
            }
            if (a && *a == 0) {
                return std::make_shared<num_expr>(0);
            }

            if (b && *b == 1) {
                return lhs;
            }

            return std::make_shared<mod_expr>(lhs, rhs);
        }

        virtual void find_input(input_t& inp, int target) const {
            auto numer = get_number(lhs_);
            auto denom = get_number(rhs_);

            auto str = lhs_->to_string();
            int aaa;
            aaa = 5;
        }

        int eval(const input_t& inp) const override {
            return lhs_->eval(inp) % rhs_->eval(inp);
        }
    };

    class eql_expr : public expr {
        expr_ptr lhs_;
        expr_ptr rhs_;
    public:
        eql_expr(expr_ptr lhs, expr_ptr rhs) :
            lhs_(lhs), rhs_(rhs)
        {}

        std::string to_string() const override {
            return std::format("( {} == {} )",
                lhs_->to_string(), rhs_->to_string()
            );
        }

        expr_ptr clone() const override {
            return std::make_shared<eql_expr>(lhs_->clone(), rhs_->clone());
        }

        expr_ptr simplify() const override {
            auto lhs = lhs_->simplify();
            auto rhs = rhs_->simplify();

            auto a = get_number(lhs);
            auto b = get_number(rhs);
            if (a && b) {
                return std::make_shared<num_expr>((*a == *b) ? 1 : 0);
            }

            return std::make_shared<eql_expr>(lhs, rhs);
        }

        int eval(const input_t& inp) const override {
            return lhs_->eval(inp) == rhs_->eval(inp) ? 1 : 0;
        }
    };

    struct eval_command_state {
        int inp_index;
        std::unordered_map<char, int> vars;
        std::array<int, 14> inp;

        eval_command_state() : inp_index(0) {
            vars['w'] = 0;
            vars['x'] = 0;
            vars['y'] = 0;
            vars['z'] = 0;
        }
    };

    void eval_inp_expr(eval_command_state& state, const command& cmd) {
        state.vars[cmd.var] = state.inp[state.inp_index++];
    }

    int eval_arg(const eval_command_state& state, std::variant<char, int> arg) {
        struct visitor {
            const eval_command_state& state;

            visitor(const eval_command_state& state) : state(state)
            {}

            int operator()(char var) const {
                return state.vars.at(var);
            }

            int operator()(int num) const {
                return num;
            }
        };

        return std::visit(visitor(state), arg);
    }

    void eval_add_expr(eval_command_state& state, const command& cmd) {
        auto arg = eval_arg(state, *cmd.arg);
        state.vars[cmd.var] = state.vars[cmd.var] + arg;
    }

    void eval_mul_expr(eval_command_state& state, const command& cmd) {
        auto arg = eval_arg(state, *cmd.arg);
        state.vars[cmd.var] = state.vars[cmd.var] * arg;
    }

    void eval_mod_expr(eval_command_state& state, const command& cmd) {
        auto arg = eval_arg(state, *cmd.arg);
        state.vars[cmd.var] = state.vars[cmd.var] % arg;
    }

    void eval_div_expr(eval_command_state& state, const command& cmd) {
        auto arg = eval_arg(state, *cmd.arg);
        state.vars[cmd.var] = state.vars[cmd.var] / arg;
    }

    void eval_eql_expr(eval_command_state& state, const command& cmd) {
        auto arg = eval_arg(state, *cmd.arg);
        state.vars[cmd.var] = (state.vars[cmd.var] == arg) ? 1 : 0;
    }

    void eval_commands(const std::vector<command>& commands, eval_command_state& state) {
        for (const auto& cmd : commands) {
            switch (cmd.op) {
            case inp:
                eval_inp_expr(state, cmd);
                break;

            case add:
                eval_add_expr(state, cmd);
                break;

            case mul:
                eval_mul_expr(state, cmd);
                break;

            case mod:
                eval_mod_expr(state, cmd);
                break;

            case div:
                eval_div_expr(state, cmd);
                break;

            case eql:
                eval_eql_expr(state, cmd);
                break;
            }
        }
    }

    struct commands_to_expr_state {
        int inp_index;
        std::unordered_map<char, expr_ptr> vars;

        commands_to_expr_state() : inp_index(0) {
            vars['w'] = std::make_shared<num_expr>(0);
            vars['x'] = std::make_shared<num_expr>(0);
            vars['y'] = std::make_shared<num_expr>(0);
            vars['z'] = std::make_shared<num_expr>(0);
        }
    };

    void build_inp_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<inp_expr>(state.inp_index++)->simplify();
    }

    expr_ptr make_arg_expr(
            const commands_to_expr_state& state, std::variant<char, int> arg) {

        struct visitor {
            const commands_to_expr_state& state;
            
            visitor(const commands_to_expr_state& state) : state(state)
            {}

            expr_ptr operator()(char var) const {
                return state.vars.at(var)->simplify();
            }

            expr_ptr operator()(int num) const {
                return std::make_shared<num_expr>(num);
            }
        };

        return std::visit(visitor(state), arg);
    }

    void build_add_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<add_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }

    void build_mul_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<mul_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }

    void build_mod_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<mod_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }


    void build_div_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<div_expr>(
            state.vars[cmd.var],
            make_arg_expr(state, *cmd.arg)
        )->simplify();
    }

    void build_eql_expr(commands_to_expr_state& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<eql_expr>(
            state.vars[cmd.var],
            make_arg_expr(state, *cmd.arg)
        )->simplify();
    }

    commands_to_expr_state build_expr(const std::vector<command>& commands) {
        commands_to_expr_state state;
        int count = 0;
        for (const auto& cmd : commands) {
            count++;
            switch (cmd.op) {
                case inp:
                    build_inp_expr(state, cmd);
                    break;

                case add:
                    build_add_expr(state, cmd);
                    break;

                case mul:
                    build_mul_expr(state, cmd);
                    break;

                case mod:
                    build_mod_expr(state, cmd);
                    break;
                
                case div:
                    build_div_expr(state, cmd);
                    break;

                case eql:
                    build_eql_expr(state, cmd);
                    break;
            }
            if (count == 18) {
                return state;
            }
        }
        return state;
    }

    int eval_z(const std::array<int, 14> inp, const std::vector<command>& cmds) {
        eval_command_state state;
        state.inp = inp;
        eval_commands(cmds, state);
        return state.vars['z'];
    }

    void print_inp(const input_t& inp) {
        for (auto digit : inp) {
            std::print("{}", digit);
        }
        std::println("");
    }
}

void aoc::y2021::day_24(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 24));
    auto commands = input | rv::transform(str_to_command) | r::to<std::vector<command>>();

    auto res = build_expr(commands);
    //input_t inp = { 9,9,9,9,9,9,9,9,9,9,9,9,9,9 };
    //res.vars['z']->find_input(inp, 0);
    //print_inp(brute_force(commands));

    std::println("{}", res.vars['z']->to_string());

    std::println("z => {}", res.vars['z']->eval({ 3,3,3,3,3,3,5,9,9,9,9,9,9,9 }));

    std::println("z => {}", eval_z({ 3,3,3,3,3,3,5,9,9,9,9,9,9,9 }, commands));

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}