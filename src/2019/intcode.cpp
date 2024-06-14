#include "../util.h"
#include "intcode.h"
#include <functional>
#include <algorithm>
#include <string>
#include <ranges>
#include <unordered_map>
#include <print>

/*------------------------------------------------------------------------------------------------*/

namespace r = std::ranges;
namespace rv = std::ranges::views;

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

    struct op_def {
        op_code code;
        int num_args;
        std::function<void(aoc::intcode_computer&, const std::vector<parameter>& params,
            aoc::input_buffer& inp)> fn;
    };

    int eval_param(const aoc::intcode_computer& icc, const parameter& p) {
        return (p.mode == immediate) ? p.val : icc.value(p.val);
    }

    void do_add_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.value(args[2].val) = eval_param(icc, args[0]) + eval_param(icc, args[1]);
    }

    void do_mult_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.value(args[2].val) = eval_param(icc, args[0]) * eval_param(icc, args[1]);
    }

    void do_inp_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.value(args.front().val) = inp.next();
    }

    void do_outp_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.show_output(eval_param(icc, args[0]));
    }

    void do_jmp_if_true_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        if (eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_jmp_if_false_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        if (!eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_less_than_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.value(args[2].val) = eval_param(icc, args[0]) < eval_param(icc, args[1]) ? 1 : 0;
    }

    void do_equals_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            aoc::input_buffer& inp) {
        icc.value(args[2].val) = eval_param(icc, args[0]) == eval_param(icc, args[1]) ? 1 : 0;
    }

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

    std::optional<instruction> parse_next_instruction(const aoc::intcode_computer& icc)  {
        auto val = icc.current_value();
        if (val == static_cast<int>(terminate_op)) {
            return {};
        }
        auto str = std::to_string(val);
        int n = str.length();
        std::string mode_str;
        op_code op = terminate_op;
        if (val < 10) {
            op = static_cast<op_code>(val);
        }
        else {
            auto op_str = str.substr(n - 2, 2);
            mode_str = str.substr(0, n - 2);
            r::reverse(mode_str);
            op = static_cast<op_code>(std::stoi(op_str));
        }
        std::vector<parameter> params;
        for (int i = 0; i < k_op_code_table.at(op).num_args; ++i) {
            char mode = (i < mode_str.size()) ? mode_str.at(i) : '0';
            int val = icc.value(icc.current_address() + i + 1);
            params.emplace_back(val, (mode == '0') ? position : immediate);
        }

        return instruction{ op, params };
    }

    std::vector<int> eval_params(
            const aoc::intcode_computer& icc,
            const std::vector<parameter>& params) {
        return params | rv::transform(
            [&](const parameter& p)->int {
                return (p.mode == immediate) ?
                    p.val :
                    icc.value(p.val);
            }
        ) | r::to<std::vector>();
    }
}

bool aoc::intcode_computer::run_one_instruction(aoc::input_buffer& inp) {

    auto instr = parse_next_instruction(*this);
    if (!instr) {
        return false;
    }

    const auto& op = k_op_code_table.at(instr->op);
    int old_prog_counter = program_counter_;
    op.fn(*this, instr->parameters, inp);
    if (old_prog_counter == program_counter_) {
        incr_prog_counter(op.num_args + 1);
    }

    return true;
}

aoc::intcode_computer::intcode_computer(const std::vector<int>& memory) :
    memory_(memory),
    program_counter_(0),
    show_output_(false) {
}

void aoc::intcode_computer::reset(const std::vector<int>& memory) {
    memory_ = memory;
    program_counter_ = 0;
    output_ = {};
    show_output_ = false;
}


void aoc::intcode_computer::set_show_output(bool v) {
    show_output_ = v;
}

void aoc::intcode_computer::show_output(int val) {
    if (show_output_) {
        std::println("{}", val);
    }
    output_ = val;
}

int aoc::intcode_computer::output() const {
    return *output_;
}

int aoc::intcode_computer::current_value() const {
    return memory_[program_counter_];
}

int aoc::intcode_computer::current_address() const {
    return program_counter_;
}

int aoc::intcode_computer::value(int i) const {
    return memory_[i];
}

int& aoc::intcode_computer::value(int i) {
    return memory_[i];
}

void aoc::intcode_computer::incr_prog_counter(int incr) {
    program_counter_ += incr;
}

void aoc::intcode_computer::jump_to(int address) {
    program_counter_ = address;
}

void aoc::intcode_computer::run(input_buffer& inp) {
    bool not_done = true;
    while (not_done) {
        not_done = run_one_instruction(inp);
    }
}

void aoc::intcode_computer::run() {
    input_buffer inp;
    bool not_done = true;
    while (not_done) {
        not_done = run_one_instruction(inp);
    }
}

/*------------------------------------------------------------------------------------------------*/

aoc::input_buffer::input_buffer(const std::vector<int>& inp) :
    buffer_{ inp },
    curr_{ 0 },
    gen_{}
{
}

aoc::input_buffer::input_buffer(const input_generator& gen) :
    buffer_{},
    curr_{ 0 },
    gen_{gen}
{
}

int aoc::input_buffer::next()
{
    if (gen_) {
        return gen_(curr_++);
    } else {
        return buffer_[curr_++];
    }
}

void aoc::input_buffer::reset()
{
    curr_ = 0;
}
