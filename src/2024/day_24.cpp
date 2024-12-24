
#include "../util/util.h"
#include "y2024.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <memory>
#include <variant>

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

    std::vector<node_ptr> build_gate_array(
            const std::vector<wire>& wires, const std::vector<gate_info>& gates) {
        std::unordered_map<std::string, node_ptr> nodes;

        for (const auto& wire : wires) {
            nodes[wire.name] = std::make_shared<node>( wire.val );
        }
        int max_z = -1;
        for (const auto& gate : gates) {
            nodes[gate.output] = std::make_shared<node>(::gate{ {}, gate.op, {}});
            if (gate.output.front() == 'z') {
                auto num = std::stoi(aoc::remove_nonnumeric(gate.output));
                max_z = std::max(max_z, num);
            }
        }

        std::vector<node_ptr> z_nodes(max_z + 1);
        for (const auto& gate_info : gates) {
            auto& g = std::get<::gate>(*nodes[gate_info.output]);
            g.lhs = nodes.at(gate_info.lhs);
            g.rhs = nodes.at(gate_info.rhs);
            if (gate_info.output.front() == 'z') {
                auto num = std::stoi(aoc::remove_nonnumeric(gate_info.output));
                z_nodes[num] = nodes[gate_info.output];
            }
        }

        return z_nodes;
    }

    bool eval_node(node_ptr ptr) {
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

    uint64_t to_uint64(const std::vector<bool>& bool_vec) {
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

    uint64_t do_part_1(std::vector<node_ptr>& ary) {
        return to_uint64( ary | rv::transform(eval_node) | r::to<std::vector>() );
    }
}

void aoc::y2024::day_24(const std::string& title) {

    auto [wires, gates] = parse_input(
        aoc::file_to_string_vector(
            aoc::input_path(2024, 24)
        )
    );

    auto gate_array = build_gate_array(wires, gates);

    std::println("--- Day 24: {} ---", title);
    std::println("  part 1: {}", do_part_1(gate_array));
    std::println("  part 2: {}", 0);
    
}
