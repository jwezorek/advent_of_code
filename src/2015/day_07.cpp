#include "../util.h"
#include "y2015.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using signal = uint16_t;

    class node;
    using node_ptr = std::shared_ptr<node>;

    class node {
        std::string label_;
        std::vector<node_ptr> inputs_;
        std::optional<signal> signal_;

        virtual signal perform_op(const std::vector<signal>& inp) const = 0;

    public:

        node(const std::string& label) : label_(label) {

        }

        std::string label() {
            return label_;
        }

        void insert_input(node_ptr inp) {
            inputs_.push_back(inp);
        }

        signal evaluate() {
            if (!signal_) {
                auto input_signals = inputs_ | rv::transform(
                    [](auto ptr)->signal {
                        return ptr->evaluate();
                    }
                ) | r::to<std::vector>();

                signal_ = perform_op(input_signals);
            }
            return *signal_;
        }

        void reset() {
            signal_ = {};
        }

        void set_signal(signal sig) {
            signal_ = sig;
        }
    };

    class input_node : public node {
        signal value_;

        signal perform_op(const std::vector<signal>& inp) const override {
            return value_;
        }

    public:
        input_node(const std::string& lbl, signal value) : node(lbl), value_(value) {

        }
    };

    class wire_node : public node {

        signal perform_op(const std::vector<signal>& inp) const override {
            return inp[0];
        }

    public:
        wire_node(const std::string& lbl) : node(lbl) {

        }
    };

    class and_node : public node {
        signal perform_op(const std::vector<signal>& inp) const override {
            return inp[0] & inp[1];
        }
    public:
        and_node(const std::string& lbl) : node(lbl) {

        }
    };

    class or_node : public node {
        signal perform_op(const std::vector<signal>& inp) const override {
            return inp[0] | inp[1];
        }
    public:
        or_node(const std::string& lbl) : node(lbl) {

        }
    };

    class lshift_node : public node {
        signal value_;

        signal perform_op(const std::vector<signal>& inp) const override {
            return inp[0] << value_;
        }
    public:
        lshift_node(const std::string& lbl, signal val) : value_(val), node(lbl) {
        }
    };

    class rshift_node : public node {
        signal value_;

        signal perform_op(const std::vector<signal>& inp) const override {
            return inp[0] >> value_;
        }
    public:
        rshift_node(const std::string& lbl, signal val) : value_(val), node(lbl) {
        }
    };

    class not_node : public node {

        signal perform_op(const std::vector<signal>& inp) const override {
            return ~inp[0];
        }
    public:
        not_node(const std::string& lbl) : node(lbl) {
        }
    };

    using input_map = std::unordered_map<std::string, std::vector<std::string>>;

    bool is_number(const std::string& s) {
        return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
    }

    node_ptr parse_input_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[1];
        if (is_number(toks[0])) {
            return std::make_shared<input_node>(lbl, std::stoi(toks[0]));
        }
        inputs[lbl] = { toks[0] };
        return std::make_shared<wire_node>(lbl);
    }

    node_ptr parse_and_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[3];
        inputs[lbl] = { toks[0], toks[2] };
        return std::make_shared<and_node>(lbl);
    }

    node_ptr parse_or_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[3];
        inputs[lbl] = { toks[0], toks[2] };
        return std::make_shared<or_node>(lbl);
    }

    node_ptr parse_lshift_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[3];
        inputs[lbl] = { toks[0] };
        return std::make_shared<lshift_node>(lbl, std::stoi(toks[2]));
    }

    node_ptr parse_rshift_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[3];
        inputs[lbl] = { toks[0] };
        return std::make_shared<rshift_node>(lbl, std::stoi(toks[2]));
    }

    node_ptr parse_not_node(const std::vector<std::string>& toks, input_map& inputs) {
        auto lbl = toks[2];
        inputs[lbl] = { toks[1] };
        return std::make_shared<not_node>(lbl);
    }

    using parse_fn = std::function<node_ptr(const std::vector<std::string>&, input_map&)>;

    node_ptr parse_inp_line(const std::string& inp_str, input_map& inputs) {

        const static std::array<std::tuple<std::string, parse_fn>, 5> parsers = {{
            {"AND", parse_and_node},
            {"OR", parse_or_node},
            {"LSHIFT", parse_lshift_node},
            {"RSHIFT", parse_rshift_node},
            {"NOT", parse_not_node}
        }};

        auto toks = aoc::extract_alphanumeric(inp_str);
        for (const auto& [keyword, parser] : parsers) {
            if (inp_str.contains(keyword)) {
                return parser(toks, inputs);
            }
        }

        return parse_input_node(toks, inputs);
    }

    using node_map = std::unordered_map<std::string, node_ptr>;
    node_map parse_input(const std::vector<std::string>& inp) {
        input_map node_to_inputs;
        auto nodes = inp | rv::transform(
                [&](const std::string& str)->node_map::value_type {
                    auto node = parse_inp_line(str, node_to_inputs);
                    return {node->label(), node};
                }
            ) | r::to<node_map>();
        for (auto node : nodes | rv::values) {
            if (!node_to_inputs.contains(node->label())) {
                continue;
            }
            for (auto node_or_num : node_to_inputs.at(node->label())) {
                if (is_number(node_or_num)) {
                    node->insert_input(std::make_shared<input_node>("", std::stoi(node_or_num)));
                } else {
                    node->insert_input(nodes.at(node_or_num));
                }
            }
        }
        return nodes;
    }
}

void aoc::y2015::day_07(const std::string& title) {

    auto inp = aoc::file_to_string_vector(aoc::input_path(2015, 7));

    auto nodes = parse_input(inp);
    signal a_val = nodes["a"]->evaluate();

    std::println("--- Day 7: {} ---", title);
    
    std::println("  part 1: {}", a_val );

    for (auto node : nodes | rv::values) {
        node->reset();
    }
    nodes["b"]->set_signal(a_val);

    std::println("  part 2: {}",
        nodes["a"]->evaluate()
    );
}