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
        equals = 8,
        set_rel_base = 9
    };

    enum param_mode {
        position,
        immediate,
        relative
    };

    struct parameter {
        int64_t val;
        param_mode mode;
    };

    struct instruction {
        op_code op;
        std::vector<parameter> parameters;
    };

    using op_fn = std::function<
        void(aoc::intcode_computer&,
            const std::vector<parameter>& params,
            const aoc::input_fn& inp, const aoc::output_fn& out
            )>;
    

    struct op_def {
        op_code code;
        int num_args;
        op_fn fn;
    };

    const int64_t& eval_param_as_ref(const aoc::intcode_computer& icc, const parameter& p) {
        if (p.mode == immediate) {
            throw std::runtime_error("attempted to write to immediate mode param");
        }
        int64_t base = (p.mode == position) ? 0 : icc.rel_base();
        return icc.value( base + p.val );
    }

    int64_t& eval_param_as_ref(aoc::intcode_computer& icc, const parameter& p) {
        return const_cast<int64_t&>(
            eval_param_as_ref( const_cast<const aoc::intcode_computer&>(icc), p )
        );
    }

    int64_t eval_param(const aoc::intcode_computer& icc, const parameter& p) {
        if (p.mode == immediate) {
            return p.val;
        }
        return eval_param_as_ref(icc, p);
    }

    void do_add_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        eval_param_as_ref(icc,args[2]) = eval_param(icc, args[0]) + eval_param(icc, args[1]);
    }

    void do_mult_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        eval_param_as_ref(icc, args[2]) = eval_param(icc, args[0]) * eval_param(icc, args[1]);
    }

    void do_inp_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        eval_param_as_ref(icc, args[0]) = inp();
    }

    void do_outp_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        auto output_val = eval_param(icc, args[0]);
        icc.set_output(output_val);
        out(output_val);
    }

    void do_jmp_if_true_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        if (eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_jmp_if_false_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        if (!eval_param(icc, args[0])) {
            icc.jump_to(eval_param(icc, args[1]));
        }
    }

    void do_less_than_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        eval_param_as_ref(icc, args[2]) = eval_param(icc, args[0]) < eval_param(icc, args[1]) ? 1 : 0;
    }

    void do_equals_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        eval_param_as_ref(icc, args[2]) = eval_param(icc, args[0]) == eval_param(icc, args[1]) ? 1 : 0;
    }

    void do_set_rel_base_op(aoc::intcode_computer& icc, const std::vector<parameter>& args,
            const aoc::input_fn& inp, const aoc::output_fn& out) {
        icc.adjust_rel_base(eval_param(icc, args[0]));
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
        {set_rel_base, {set_rel_base, 1, do_set_rel_base_op}}
    };

    param_mode char_to_param_mode(char ch) {
        const static std::unordered_map<char, param_mode> tbl = {
            {'0', position},
            {'1', immediate},
            {'2', relative}
        };
        return tbl.at(ch);
    }

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
            char mode_char = (i < mode_str.size()) ? mode_str.at(i) : '0';
            int val = icc.value(icc.current_address() + i + 1);
            params.emplace_back(val, char_to_param_mode(mode_char));
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

    std::unordered_map<int64_t, int64_t> to_memory_table(const std::vector<int64_t>& code) {
        return rv::enumerate(code) | rv::transform(
                [](const auto& addr_val)->std::unordered_map<int64_t, int64_t>::value_type {
                    auto [address, val] = addr_val;
                    return { address, val };
                }
            ) | r::to<std::unordered_map<int64_t, int64_t>>();
    }
}

bool aoc::intcode_computer::run_one_instruction(
        const input_fn& inp, const output_fn& out) {

    auto instr = parse_next_instruction(*this);
    if (!instr) {
        return false;
    }

    const auto& op = k_op_code_table.at(instr->op);
    int old_prog_counter = program_counter_;
    op.fn(*this, instr->parameters, inp, out);
    if (old_prog_counter == program_counter_) {
        incr_prog_counter(op.num_args + 1);
    }

    return true;
}

aoc::intcode_computer::intcode_computer(const std::vector<int64_t>& memory) :
    memory_( to_memory_table(memory) ),
    program_counter_(0),
    rel_base_(0) {
}

void aoc::intcode_computer::reset(const std::vector<int64_t>& memory) {
    memory_ = to_memory_table(memory);
    program_counter_ = 0;
    output_ = {};
    rel_base_ = 0;
}

void aoc::intcode_computer::set_output(int64_t v) {
    output_ = v;
}

void aoc::intcode_computer::adjust_rel_base(int64_t offset) {
    rel_base_ += offset;
}

int64_t aoc::intcode_computer::rel_base() const {
    return rel_base_;
}

int64_t aoc::intcode_computer::output() const {
    return *output_;
}

int64_t aoc::intcode_computer::current_value() const {
    return value( program_counter_ );
}

int64_t aoc::intcode_computer::current_address() const {
    return program_counter_;
}

const int64_t& aoc::intcode_computer::value(int64_t i) const {
    if (i < 0) {
        throw std::runtime_error( "bad memory access" );
    }
    if (!memory_.contains(i)) {
        memory_[i] = 0; // memory_ is mutable so this is possible...
    }
    return memory_.at(i);
}

int64_t& aoc::intcode_computer::value(int64_t i) {
    return const_cast<int64_t&>(
        const_cast<const aoc::intcode_computer*>(this)->value(i)
    );
}

void aoc::intcode_computer::incr_prog_counter(int64_t incr) {
    program_counter_ += incr;
}

void aoc::intcode_computer::jump_to(int64_t address) {
    program_counter_ = address;
}

void aoc::intcode_computer::run(input_buffer& inp) {
    run(
        [&inp]()->int64_t {
            return inp.next();
        },
        [this](int64_t val) { }
    );
}

void aoc::intcode_computer::run() {
    input_buffer inp;
    run(inp);
}

void aoc::intcode_computer::run(const input_fn& inp, const output_fn& out)
{
    bool not_done = true;
    while (not_done) {
        not_done = run_one_instruction(inp, out);
    }
}

aoc::icc_event aoc::intcode_computer::run_until_event(int64_t inp)
{
    std::optional<icc_event> event = {};
    while (!event) {
        bool not_done = run_one_instruction(
            [&]() {
                event = received_input;
                return inp; 
            },
            [&](int64_t out) {
                event = generated_output;
            }
        );
        if (not_done == false) {
            event = terminated;
        }
    }
    return *event;
}

/*------------------------------------------------------------------------------------------------*/

aoc::input_buffer::input_buffer(const std::vector<int64_t>& inp) :
    buffer_{ inp },
    curr_{ 0 }
{
}

int64_t aoc::input_buffer::next()
{
    return buffer_[curr_++];
}

void aoc::input_buffer::reset()
{
    curr_ = 0;
}
