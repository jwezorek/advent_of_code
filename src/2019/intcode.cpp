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

namespace aoc {
    class impl {
    public:

        using computer = intcode_computer;

        enum op_code {
            terminate_op = 99,
            add_op = 1,
            mult_op = 2,
            inp_op = 3,
            outp_op = 4,
            jmp_if_true_op = 5,
            jmp_if_false_op = 6,
            less_than_op = 7,
            equals_op = 8,
            set_rel_base_op = 9
        };

        enum param_mode {
            position = 0,
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
            void(computer&,
                const std::vector<parameter>& params,
                const input_fn& inp, const output_fn& out
                )>;


        struct op_def {
            op_code code;
            int num_args;
            op_fn fn;
        };

        static const int64_t& eval_param_as_ref( const computer& icc, const parameter& p) {
            if (p.mode == immediate) {
                throw std::runtime_error("attempted to write to immediate mode param");
            }
            int64_t base = (p.mode == position) ? 0 : icc.rel_base_;
            return icc.value(base + p.val);
        }

        static int64_t& eval_param_as_ref(computer& icc, const parameter& p) {
            return const_cast<int64_t&>(
                eval_param_as_ref(const_cast<const computer&>(icc), p)
            );
        }

        static int64_t eval_param(const computer& icc, const parameter& p) {
            if (p.mode == immediate) {
                return p.val;
            }
            return eval_param_as_ref(icc, p);
        }

        static void do_add_op(computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            eval_param_as_ref(icc, args[2]) = eval_param(icc, args[0]) + eval_param(icc, args[1]);
        }

        static void do_mult_op(computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            eval_param_as_ref(icc, args[2]) = eval_param(icc, args[0]) * eval_param(icc, args[1]);
        }

        static void do_inp_op(computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            eval_param_as_ref(icc, args[0]) = inp();
        }

        static void do_outp_op(computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            auto output_val = eval_param(icc, args[0]);
            icc.output_ = output_val;
            out(output_val);
        }

        static void do_jmp_if_true_op( computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            if (eval_param(icc, args[0])) {
                icc.program_counter_ = eval_param(icc, args[1]);
            }
        }

        static void do_jmp_if_false_op( computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            if (!eval_param(icc, args[0])) {
                icc.program_counter_ = eval_param(icc, args[1]);
            }
        }

        static void do_less_than_op( computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            eval_param_as_ref(icc, args[2]) = 
                eval_param(icc, args[0]) < eval_param(icc, args[1]) ? 1 : 0;
        }

        static void do_equals_op( computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {

            eval_param_as_ref(icc, args[2]) = 
                eval_param(icc, args[0]) == eval_param(icc, args[1]) ? 1 : 0;
        }

        static void do_set_rel_base_op(
                computer& icc, const std::vector<parameter>& args,
                const input_fn& inp, const output_fn& out) {
            icc.rel_base_ += eval_param(icc, args[0]);
        }

        static int64_t current_value(const computer& icc) {
            return icc.value(icc.program_counter_);
        }

        static int64_t current_addr(const computer& icc) {
            return icc.program_counter_;
        }
    };
}

namespace {

    using impl = aoc::impl;
    using computer = aoc::intcode_computer;

    const std::unordered_map<impl::op_code, impl::op_def> k_op_code_table = {
        {impl::add_op, {impl::add_op, 3, impl::do_add_op}},
        {impl::mult_op, {impl::mult_op, 3, impl::do_mult_op}},
        {impl::inp_op, {impl::add_op, 1, impl::do_inp_op}},
        {impl::outp_op, {impl::mult_op, 1, impl::do_outp_op}},
        {impl::jmp_if_true_op, {impl::jmp_if_true_op, 2, impl::do_jmp_if_true_op}},
        {impl::jmp_if_false_op, {impl::jmp_if_false_op, 2, impl::do_jmp_if_false_op}},
        {impl::less_than_op, {impl::less_than_op, 3, impl::do_less_than_op}},
        {impl::equals_op, {impl::equals_op, 3, impl::do_equals_op}},
        {impl::set_rel_base_op, {impl::set_rel_base_op, 1, impl::do_set_rel_base_op}}
    };

    impl::param_mode char_to_param_mode(char ch) {
        return static_cast<impl::param_mode>(ch - '0');
    }

    std::optional<impl::instruction> parse_next_instruction(computer& icc)  {

        auto val = impl::current_value(icc);
        if (val == static_cast<int>(impl::terminate_op)) {
            return {};
        }

        auto str = std::to_string(val);
        int n = static_cast<int>(str.length());
        std::string mode_str;
        impl::op_code op = impl::terminate_op;
        if (val < 10) {
            op = static_cast<impl::op_code>(val);
        } else {
            auto op_str = str.substr(n - 2, 2);
            mode_str = str.substr(0, n - 2);
            r::reverse(mode_str);
            op = static_cast<impl::op_code>(std::stoi(op_str));
        }

        std::vector<impl::parameter> params;
        for (int i = 0; i < k_op_code_table.at(op).num_args; ++i) {
            params.emplace_back(
                icc.value(aoc::impl::current_addr(icc) + i + 1),
                char_to_param_mode((i < mode_str.size()) ? mode_str.at(i) : '0')
            );
        }

        return impl::instruction{ op, params };
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
    auto old_prog_counter = program_counter_;
    op.fn(*this, instr->parameters, inp, out);
    if (old_prog_counter == program_counter_) {
        program_counter_ += op.num_args + 1;
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

int64_t aoc::intcode_computer::output() const {
    return *output_;
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

void aoc::intcode_computer::run(const input_fn& inp, const output_fn& out) {
    bool not_done = true;
    while (not_done) {
        not_done = run_one_instruction(inp, out);
    }
}

aoc::icc_event aoc::intcode_computer::run_until_event(std::optional<int64_t> inp) {
    std::optional<icc_event> event = {};
    while (!event) {

        auto next_instruction = parse_next_instruction(*this);
        if (next_instruction && next_instruction->op == impl::inp_op && !inp) {
            return awaiting_input;
        }

        bool not_done = run_one_instruction(
            [&]() {
                if (!inp) {
                    throw std::runtime_error( "expected input" );
                }
                event = received_input;
                return *inp; 
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

int64_t aoc::input_buffer::next() {
    return buffer_[curr_++];
}

void aoc::input_buffer::reset() {
    curr_ = 0;
}
