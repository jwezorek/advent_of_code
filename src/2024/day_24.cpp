
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>
#include <variant>
#include <fstream>
#include <iostream>


namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    enum op {
        and_,
        or_,
        xor_
    };

    struct node;

    using node_ptr = std::shared_ptr<node>;

    struct gate {
        node_ptr lhs;
        op op;
        node_ptr rhs;
    };

    struct node : public std::variant<bool, gate> {
        using base = std::variant<bool, gate>;
        using base::base;
        using base::operator=;
    };

    struct wire {
        std::string name;
        bool val;
    };

    struct gate_info {
        std::string output;
        std::string lhs;
        op op;
        std::string rhs;
    };

    std::tuple<std::vector<wire>, std::vector<gate_info>> parse_input(
            const std::vector<std::string>& inp) {
        auto groups = aoc::group_strings_separated_by_blank_lines(inp);
        auto wires = groups[0] | rv::transform(
                [](auto&& str)->wire {
                    auto parts = aoc::extract_alphanumeric(str);
                    return {
                        parts[0],
                        static_cast<bool>(std::stoi(parts[1]))
                    };
                }
            ) | r::to<std::vector>();
        auto gates = groups[1] | rv::transform(
            [](auto&& str)->gate_info {
                auto parts = aoc::extract_alphanumeric(str);
                const static std::unordered_map<std::string, op> op_tbl = {
                    {"AND", and_},
                    {"OR", or_},
                    {"XOR", xor_}
                };
                return {
                    parts[3],
                    parts[0],
                    op_tbl.at(parts[1]),
                    parts[2]
                };
            }
        ) | r::to<std::vector>();

        return { std::move(wires), std::move(gates) };
    }

    class broken_adder {
    private:
        std::vector<node_ptr> output_;
        std::vector<node_ptr> x_input_;
        std::vector<node_ptr> y_input_;

        static uint64_t to_uint64(const std::vector<bool>& bool_vec) {
            if (bool_vec.size() > 64) {
                throw std::invalid_argument("The vector size exceeds 64 bits.");
            }

            uint64_t result = 0;
            for (size_t i = 0; i < bool_vec.size(); ++i) {
                if (bool_vec[i]) {
                    result |= (uint64_t(1) << i);
                }
            }
            return result;
        }

        std::vector<bool> uint64_to_bool_vector(uint64_t value) {
            std::vector<bool> bool_vec;
            for (size_t i = 0; i < 64; ++i) {
                bool_vec.push_back((value & (uint64_t(1) << i)) != 0);
            }
            return bool_vec;
        }

        static bool eval_node(node_ptr ptr) {
            const auto& node = *ptr;
            return std::visit(
                aoc::overload{
                    [](const gate& g)->bool {
                        bool lhs = eval_node(g.lhs);
                        bool rhs = eval_node(g.rhs);
                        switch (g.op) {
                            case and_:
                                return lhs && rhs;
                            case or_:
                                return lhs || rhs;
                            case xor_:
                                return lhs != rhs;
                        }
                        std::unreachable();
                    },
                    [](bool val)->bool {
                        return val;
                    }
                },
                node
            );
        }

    public:
        broken_adder(const std::vector<wire>& wires, const std::vector<gate_info>& gates) :
                output_(46, nullptr),
                y_input_(46, nullptr),
                x_input_(46, nullptr) {

            std::unordered_map<std::string, node_ptr> nodes;

            for (const auto& wire : wires) {
                nodes[wire.name] = std::make_shared<node>(wire.val);
                if (wire.name.front() == 'x') {
                    auto num = std::stoi(aoc::remove_nonnumeric(wire.name));
                    x_input_[num] = nodes[wire.name];
                } else if (wire.name.front() == 'y') {
                    auto num = std::stoi(aoc::remove_nonnumeric(wire.name));
                    y_input_[num] = nodes[wire.name];
                }
            }

            for (const auto& gate : gates) {
                nodes[gate.output] = std::make_shared<node>(::gate{ {}, gate.op, {} });
            }

            for (const auto& gate_info : gates) {
                auto& g = std::get<::gate>(*nodes[gate_info.output]);
                g.lhs = nodes.at(gate_info.lhs);
                g.rhs = nodes.at(gate_info.rhs);
                if (gate_info.output.front() == 'z') {
                    auto num = std::stoi(aoc::remove_nonnumeric(gate_info.output));
                    output_[num] = nodes[gate_info.output];
                }
            }
        }

        uint64_t eval() {
            return to_uint64(
                output_ | rv::filter(
                    [](auto&& ptr)->bool {
                        return ptr.get();
                    }
                ) | rv::transform(
                    eval_node
                ) | r::to<std::vector>());
        }

        uint64_t add(uint64_t x, uint64_t y) {
            auto x_vec = uint64_to_bool_vector(x);
            auto y_vec = uint64_to_bool_vector(y);

            for (int i = 0; i < 45; ++i) {
                if (i < x_vec.size()) {
                    *x_input_[i] = node{ x_vec[i] };
                } else {
                    *x_input_[i] = node{ false };
                }
                if (i < y_vec.size()) {
                    *y_input_[i] = node{ y_vec[i] };
                } else {
                    *y_input_[i] = node{ false };
                }
            }

            return eval();
        }
    };

    void generate_dot_file(const std::vector<wire>& wires, const std::vector<gate_info>& gates, const std::string& path) {
        std::ofstream file(path);

        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << path << std::endl;
            return;
        }

        // Start of the graph
        file << "digraph circuit {\n";
        file << "    rankdir=LR;\n";

        // Add wire nodes
        for (const auto& w : wires) {
            file << "    \"" << w.name << "\" [label=\"" << w.name << "\\n" << (w.val ? "1" : "0") << "\", shape=circle];\n";
        }

        // Add gate nodes and connections
        for (const auto& g : gates) {
            std::string op_label;
            switch (g.op) {
            case and_: op_label = "AND"; break;
            case or_: op_label = "OR"; break;
            case xor_: op_label = "XOR"; break;
            }

            // Create the gate node
            file << "    \"" << g.output << "_gate\" [label=\"" << op_label << "\", shape=box];\n";

            // Connect inputs to the gate
            file << "    \"" << g.lhs << "\" -> \"" << g.output << "_gate\";\n";
            file << "    \"" << g.rhs << "\" -> \"" << g.output << "_gate\";\n";

            // Connect the gate to the output
            file << "    \"" << g.output << "_gate\" -> \"" << g.output << "\";\n";
        }

        // End of the graph
        file << "}\n";

        file.close();
        std::cout << "DOT file generated successfully at: " << path << std::endl;
    }

    uint64_t generateBinaryOnes(int n) {
        if (n < 1 || n > 64) {
            throw std::out_of_range("n must be between 1 and 64.");
        }

        return (n == 64) ? ~uint64_t(0) : (uint64_t(1) << n) - 1;
    }

    std::vector<int> find_bad_bits(broken_adder& adder) {
        std::vector<int> bad_bits;
        
        for (int i = 1; i < 45; ++i) {
            auto val = generateBinaryOnes(i);
            auto sum = adder.add(val, 1);
            std::println("{} + 1 == {}  {}", val, sum,
                sum != val + 1 ? "*" : ""
            );

            if (sum != val + 1) {
                std::println("{}", i);
            }
        }

        return bad_bits;
    }
}

void aoc::y2024::day_24(const std::string& title) {

    auto [wires, gates] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 24, "test")
        )
    );

    broken_adder adder(wires, gates);

   // find_bad_bits(adder);

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", adder.eval());
    std::println("  part 2: {}", "<figured it out from GraphViz output>");
    
}
