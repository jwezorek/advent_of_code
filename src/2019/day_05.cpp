#include "../util.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <optional>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum op_code {
        terminate_op = 99,
        add_op = 1,
        mult_op = 2,
        inp_op = 3,
        outp_op = 4,
        jmp_if_true = 5,
        jmp_if_false = 6,
        less_than = 7,
        equals = 8
    };

    enum param_mode {
        position,
        immediate
    };

    struct parameter {
        int val;
        param_mode mode;
    };

    struct instruction {
        op_code op;
        std::vector<parameter> parameters;
    };

    class intcode_computer;

    struct op_def {
        op_code code;
        int num_args;
        std::function<void(intcode_computer&, const std::vector<parameter>& params)> fn;
    };

    void do_add_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_mult_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_inp_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_outp_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_jmp_if_true_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_jmp_if_false_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_less_than_op(intcode_computer& icc, const std::vector<parameter>& args);
    void do_equals_op(intcode_computer& icc, const std::vector<parameter>& args);

    const std::unordered_map<op_code, op_def> k_op_code_table = {
        {add_op, {add_op, 3, do_add_op}},
        {mult_op, {mult_op, 3, do_mult_op}},
        {inp_op, {add_op, 1, do_inp_op}},
        {outp_op, {mult_op, 1, do_outp_op}},
        {jmp_if_true, {jmp_if_true, 2, do_jmp_if_true_op}},
        {jmp_if_false, {jmp_if_false, 2, do_jmp_if_false_op}},
        {less_than, {less_than, 3, do_less_than_op}},
        {equals, {equals, 3, do_equals_op}},
    };

    class intcode_computer {
        std::vector<int> memory_;
        int program_counter_;
        std::optional<int> inp_;
        bool show_output_;
        std::optional<int> output_;

        std::optional<instruction> parse_next_instruction() const {
            auto val = current_value();
            if (val == static_cast<int>(terminate_op)) {
                return {};
            }
            auto str = std::to_string(val);
            int n = str.length();
            std::string mode_str;
            op_code op = terminate_op;
            if (val < 10) {
                op = static_cast<op_code>(val);
            } else {
                auto op_str = str.substr(n - 2, 2);
                mode_str = str.substr(0, n - 2);
                r::reverse(mode_str);
                op = static_cast<op_code>(std::stoi(op_str));
            }
            std::vector<parameter> params;
            for (int i = 0; i < k_op_code_table.at(op).num_args; ++i) {
                char mode = (i < mode_str.size()) ? mode_str.at(i) : '0';
                int val = value(program_counter_ + i + 1);
                params.emplace_back(val, (mode == '0') ? position : immediate);
            }

            return instruction{ op, params };
        }

        std::vector<int> eval_params(const std::vector<parameter>& params) const {
            return params | rv::transform(
                    [&](const parameter& p)->int {
                        return (p.mode == immediate) ?
                            p.val :
                            value(p.val);
                    }
                ) | r::to<std::vector>();
        }

        bool run_one_instruction() {

            auto instr = parse_next_instruction();
            if (!instr) {
                return false;
            }

            const auto& op = k_op_code_table.at(instr->op);
            int old_prog_counter = program_counter_;
            op.fn(*this, instr->parameters);
            if (old_prog_counter == program_counter_) {
                incr_prog_counter(op.num_args + 1);
            }

            return true;
        }

    public:

        intcode_computer(const std::vector<int>&memory) :
            memory_(memory),
            program_counter_(0),
            show_output_(false) {
        }

        void reset(const std::vector<int>& memory) {
            memory_ = memory;
            program_counter_ = 0;
            inp_ = {};
            output_ = {};
            show_output_ = false;
        }

        void set_input(int inp) {
            inp_ = inp;
        }


        void set_show_output(bool v) {
            show_output_ = v;
        }

        void show_output(int val) {
            if (show_output_) {
                std::println("{}", val);
            }
            output_ = val;
        }

        int output() const {
            return *output_;
        }

        int input() const {
            return *inp_;
        }

        int current_value() const {
            return memory_[program_counter_];
        }

        int value(int i) const {
            return memory_[i];
        }

        int& value(int i) {
            return memory_[i];
        }

        void incr_prog_counter(int incr) {
            program_counter_ += incr;
        }

        void jump_to(int address) {
            program_counter_ = address;
        }

        void run() {
            bool not_done = true;
            while (not_done) {
                not_done = run_one_instruction();
            }
        }
    };

    int eval_param(const intcode_computer& icc, const parameter& p) {
        return (p.mode == immediate) ? p.val : icc.value(p.val);
    }

    void do_add_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.value(args[2].val) = eval_param(icc,args[0]) + eval_param(icc, args[1]);
    }

    void do_mult_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.value(args[2].val) = eval_param(icc, args[0]) * eval_param(icc, args[1]);
    }

    void do_inp_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.value( args.front().val ) = icc.input();
    }

    void do_outp_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.show_output( eval_param(icc, args[0]) );
    }

    void do_jmp_if_true_op(intcode_computer& icc, const std::vector<parameter>& args) {
        if (eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_jmp_if_false_op(intcode_computer& icc, const std::vector<parameter>& args) {
        if (! eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_less_than_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.value(args[2].val) = eval_param(icc, args[0]) < eval_param(icc, args[1]) ? 1 : 0;
    }

    void do_equals_op(intcode_computer& icc, const std::vector<parameter>& args) {
        icc.value(args[2].val) = eval_param(icc, args[0]) == eval_param(icc, args[1]) ? 1 : 0;
    }
}

void aoc::y2019::day_05(const std::string& title) {

    auto inp = split(
            aoc::file_to_string(aoc::input_path(2019, 5)), ','
        ) | rv::transform(
            [](auto&& str)->int {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    intcode_computer icc(inp);
    icc.set_input(1);
    icc.run();

    std::println("--- Day 5: {} ---", title);
    std::println("  part 1: {}",
        icc.output()
    );

    icc.reset(inp);
    icc.set_input(5);
    icc.run();

    std::println("  part 2: {}",
        icc.output()
    );
}