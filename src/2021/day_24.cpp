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
    struct eval_context {
        input_t inp;
        std::unordered_map<char, int> vars;
    };

    class expr {
    public:
        virtual std::string to_string() const = 0;
        virtual std::shared_ptr<expr> clone() const = 0;
        virtual std::shared_ptr<expr> simplify() const {
            return clone();
        }
        virtual int eval(const eval_context& inp) const = 0;
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

        int eval(const eval_context& inp) const override {
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


        expr_ptr clone() const override {
            return std::make_shared<inp_expr>(index_);
        }

        int eval(const eval_context& ctxt) const override {
            return ctxt.inp[index_];
        }
    };

    class var_expr : public expr {
        char var_;
    public:
        var_expr(int var) : var_(var) {}

        std::string to_string() const override {
            return std::format("{}", var_);
        };


        expr_ptr clone() const override {
            return std::make_shared<var_expr>(var_);
        }

        int eval(const eval_context& ctxt) const override {
            return ctxt.vars.at(var_);
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

        int eval(const eval_context& ctxt) const override {
            return lhs_->eval(ctxt) + rhs_->eval(ctxt);
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

        int eval(const eval_context& ctxt) const override {
            return lhs_->eval(ctxt) * rhs_->eval(ctxt);
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

        int eval(const eval_context& ctxt) const override {
            return lhs_->eval(ctxt) / rhs_->eval(ctxt);
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

        int eval(const eval_context& ctxt) const override {
            return lhs_->eval(ctxt) % rhs_->eval(ctxt);
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

        int eval(const eval_context& ctxt) const override {
            return lhs_->eval(ctxt) == rhs_->eval(ctxt) ? 1 : 0;
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

    struct build_expr_context {
        std::unordered_map<char, expr_ptr> vars;

        build_expr_context() {
            vars['w'] = std::make_shared<num_expr>(0);
            vars['x'] = std::make_shared<num_expr>(0);
            vars['y'] = std::make_shared<num_expr>(0);
            vars['z'] = std::make_shared<num_expr>(0);
        }

        build_expr_context(int i, expr_ptr w, expr_ptr x, expr_ptr y, expr_ptr z) {
            vars['w'] = w;
            vars['x'] = x;
            vars['y'] = y;
            vars['z'] = z;
        }
    };

    void build_inp_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<var_expr>('i');
    }

    expr_ptr make_arg_expr(
            const build_expr_context& state, std::variant<char, int> arg) {

        struct visitor {
            const build_expr_context& state;
            
            visitor(const build_expr_context& state) : state(state)
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

    void build_add_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<add_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }

    void build_mul_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<mul_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }

    void build_mod_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<mod_expr>(
                state.vars[cmd.var],
                make_arg_expr(state, *cmd.arg)
            )->simplify();
    }


    void build_div_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<div_expr>(
            state.vars[cmd.var],
            make_arg_expr(state, *cmd.arg)
        )->simplify();
    }

    void build_eql_expr(build_expr_context& state, command cmd) {
        state.vars[cmd.var] = std::make_shared<eql_expr>(
            state.vars[cmd.var],
            make_arg_expr(state, *cmd.arg)
        )->simplify();
    }

    void build_expr(const std::vector<command>& commands, build_expr_context& ctxt) {
        for (const auto& cmd : commands) {
            switch (cmd.op) {
                case inp:
                    build_inp_expr(ctxt, cmd);
                    break;

                case add:
                    build_add_expr(ctxt, cmd);
                    break;

                case mul:
                    build_mul_expr(ctxt, cmd);
                    break;

                case mod:
                    build_mod_expr(ctxt, cmd);
                    break;
                
                case div:
                    build_div_expr(ctxt, cmd);
                    break;

                case eql:
                    build_eql_expr(ctxt, cmd);
                    break;
            }
        }
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

    class subroutine {
        expr_ptr z;
    public:
        subroutine(const std::vector<command>& cmds, int inp_index) {
            build_expr_context ctxt(inp_index,
                std::make_shared<var_expr>('w'),
                std::make_shared<var_expr>('x'),
                std::make_shared<var_expr>('y'),
                std::make_shared<var_expr>('z')
            );
            build_expr(cmds, ctxt);
            z = ctxt.vars.at('z');
        }

        void display() {
            std::println("z => {}",  z->to_string());
        }

        int eval(int z, int inp) {
            eval_context ctxt;
            ctxt.vars['z'] = z;
            ctxt.vars['i'] = inp;
        }
    };
}

void aoc::y2021::day_24(const std::string& title) {
    auto input = aoc::file_to_string_vector(aoc::input_path(2021, 24));
    auto commands = input | rv::transform(str_to_command) | r::to<std::vector<command>>();

    int subroutine_sz = static_cast<int>(commands.size()) / 14;
    auto subroutine_cmds = commands | rv::chunk(subroutine_sz) | r::to<std::vector<std::vector<command>>>();
    auto subroutines = rv::enumerate(subroutine_cmds) | rv::transform(
            [](auto&& i_cmds) {
                const auto& [i, cmds] = i_cmds;
                return subroutine(cmds, i);
            }
        ) | r::to<std::vector<subroutine>>();

    for (auto&& sub : subroutines) {
        sub.display();
    }
   

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", 0);
    std::println("  part 2: {}", 0);
}