#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <functional>
#include <queue>

#include <format>
#include <fstream>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    enum class module_type {
        broadcaster,
        flipflop,
        conjunction,
        none
    };

    using mod_info = std::tuple<std::string, module_type >;
    using edge = std::tuple<std::string, std::string>;

    module_type  parse_module_type(const std::string& line) {
        if (line.front() == '%') {
            return module_type::flipflop;
        } else if (line.front() == '&') {
            return module_type::conjunction;
        } else if (line.contains("broadcaster")) {
            return module_type::broadcaster;
        }
        return module_type::none;
    }

    std::tuple<mod_info, std::vector<edge>> parse_line(const std::string& line) {
        auto type = parse_module_type(line);
        auto nodes = aoc::extract_alphabetic(line);
        auto u = nodes.front();
        return {
            {u, type},
            nodes | rv::drop(1) | rv::transform(
                [u](auto&& v)->edge {
                    return {u, v};
                }
            ) | r::to<std::vector<edge>>()
        };
    }

    std::tuple<std::vector<mod_info>, std::vector<edge>> parse_input(
            const std::vector<std::string>& inp) {
        auto line_info = inp | rv::transform(parse_line) | 
            r::to<std::vector<std::tuple<mod_info, std::vector<edge>>>>();
        return {
            line_info | rv::keys | r::to<std::vector<mod_info>>(),
            line_info | rv::values | rv::join | r::to<std::vector<edge>>()
        };
    }

    enum signal_type {
        low, high
    };

    struct signal {
        std::string src;
        std::string dest;
        signal_type level;
    };

    class base_module {
    protected:
        std::string name_;
        std::vector<std::string> destinations_;
    public:
        base_module(const std::string& name) : name_(name) {}

        std::string name() const { 
            return name_; 
        }

        virtual std::string shape() const {
            return "plaintext";
        }

        std::vector<std::string> destinations() const { 
            return destinations_; 
        }

        virtual void add_source(const std::string& src) {
        }

        virtual void initialize(){}

        void add_destination(const std::string& dest) {
            destinations_.push_back(dest);
        }

        virtual std::vector<signal> receive_signal(const signal& sig) {
            return {};
        }

        virtual ~base_module(){}
    };

    class conjunction : public base_module {
        std::unordered_map<std::string, signal_type> input_state_;
    public:
        conjunction(const std::string& name) : base_module(name) {}

        void add_source(const std::string& src) override {
            input_state_[src] = low;
        }

        virtual void initialize() {
            for (auto& itm : input_state_) {
                itm.second = low;
            }
        }

        virtual std::string shape() const override {
            return "circle";
        }

        std::vector<signal> receive_signal(const signal& sig) override {
            input_state_[sig.src] = sig.level;
            auto all_high = r::all_of(
                input_state_ | rv::values, 
                [](signal_type lvl) {return lvl == high; }
            );

            signal_type output_level = all_high ? low : high;
            return destinations_ | rv::transform(
                [&](auto&& dest)->signal {
                    return { name_, dest, output_level };
                }
            ) | r::to<std::vector<signal>>();
        }
    };

    class flipflop : public base_module {
        bool is_on_;
    public:
        flipflop(const std::string& name) : 
            base_module(name), is_on_(false)
        {}

        virtual void initialize() {
            is_on_ = false;
        }

        virtual std::string shape() const override {
            return "diamond";
        }

        std::vector<signal> receive_signal(const signal& sig) override {
            if (sig.level == high) {
                return {};
            }
            
            is_on_ = !is_on_;
            auto pulse = is_on_ ? high : low;

            return destinations_ | rv::transform(
                [&](auto&& dest)->signal {
                    return { name_, dest, pulse };
                }
            ) | r::to<std::vector<signal>>();
        }
    };

    class broadcaster : public base_module {
    public:
        broadcaster(const std::string& name) : base_module(name) {}
        std::vector<signal> receive_signal(const signal& sig) override {
            return destinations_ | rv::transform(
                [&](auto&& dest)->signal {
                    return { name_, dest, sig.level };
                }
            ) | r::to<std::vector<signal>>();
        }

        virtual std::string shape() const override {
            return "doublecircle";
        }
    };

    using module_graph = std::unordered_map<std::string, std::unique_ptr<base_module>>;

    std::unique_ptr<base_module> make_module(module_type type, const std::string& name) {
        using factory_fn = std::function<std::unique_ptr<base_module>(const std::string&)>;
        static const std::unordered_map<module_type, factory_fn> factory = {
            {module_type::broadcaster,
                [](const std::string& n) { return std::make_unique<broadcaster>(n); }},

            {module_type::flipflop,
                [](const std::string& n) { return std::make_unique<flipflop>(n); }},

            {module_type::conjunction,
                [](const std::string& n) { return std::make_unique<conjunction>(n); }},

            {module_type::none,
                [](const std::string& n) { return std::make_unique<base_module>(n); }}
        };
        return factory.at(type)(name);
    }

    module_graph build_module_graph(const std::vector<mod_info>& verts, const std::vector<edge>& edges) {
        auto graph = verts | rv::transform(
                [](auto&& mi)->module_graph::value_type {
                    auto [name, type] = mi;
                    return { name, make_module(type, name) };
                } 
            ) | r::to< module_graph>();
        for (const auto& [u, v] : edges) {
            graph[u]->add_destination(v);
            if (graph.contains(v)) {
                graph[v]->add_source(u);
            }  else {
                graph[v] = std::make_unique<base_module>(v);
            }
        }
        return std::move(graph);
    }

    void initialize_graph(module_graph& g) {
        for (auto& itm : g) {
            itm.second->initialize();
        }
    }

    std::tuple<int,int> count_signals(module_graph& g) {
        std::queue<signal> queue;
        queue.push({ "button", "broadcaster", low });
        int count_low = 0;
        int count_high = 0;

        while (!queue.empty()) {
            auto signal = queue.front();
            queue.pop();

            if (signal.level == low) {
                ++count_low;
            } else {
                ++count_high;
            }

            auto emitted_sigs = g[signal.dest]->receive_signal(signal);
            for (const auto& emitted : emitted_sigs) {
                queue.push(emitted);
            }
        }

        return { count_low, count_high };
    }

    int64_t do_part_1( module_graph& graph) {
        initialize_graph(graph);

        int64_t low_total = 0;
        int64_t high_total = 0;

        for (int i = 0; i < 1000; ++i) {
            auto [low_count, high_count] = count_signals(graph);
            low_total += low_count;
            high_total += high_count;
        }

        return low_total * high_total;
    }

    int64_t find_cycle(module_graph& graph, std::string from) {
        initialize_graph(graph);

        int64_t button_count = 0;
        int first_count = 0;
        int second_count = 0;

        while (second_count == 0) {
            button_count++;
            std::queue<signal> queue;
            queue.push({ "button", "broadcaster", low });

            while (!queue.empty()) {
                auto signal = queue.front();
                queue.pop();

                if (signal.src == from && signal.level == high) {
                    if (first_count == 0) {
                        first_count = button_count;
                    } else {
                        second_count = button_count;
                    }
                    button_count = 0;
                }

                auto emitted_sigs = graph[signal.dest]->receive_signal(signal);
                for (const auto& emitted : emitted_sigs) {
                    queue.push(emitted);
                }
            }
        }
        if (first_count != second_count) {
            throw std::runtime_error("more complicated cycle");
        }

        return first_count;
    }

    void generate_dot_file(const std::string& fname, const module_graph& graph) {
        std::ofstream out(fname);

        out << "digraph aoc_day_20 {\n";
        for (const auto& u : graph | rv::values) {
            out << std::format("    {0} [shape=\"{1}\"]\n", u->name(), u->shape());
        }

        for (const auto& u : graph | rv::values) {
            for (const auto& v : u->destinations()) {
                out << std::format("    {0} -> {1}\n", u->name(), v);
            }
        }
        out << "}\n";

        out.close();
    }
}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_20(const std::string& title) {

    auto [verts, edges] = parse_input(aoc::file_to_string_vector(aoc::input_path(2023, 20)));
    auto graph = build_module_graph(verts, edges);

    std::println("--- Day 20: {0} ---\n", title);
    std::println("  part 1: {}", do_part_1(graph));

    int64_t xm, nh, dr, tr;
    xm = find_cycle(graph, "xm");
    nh = find_cycle(graph, "nh");
    dr = find_cycle(graph, "dr");
    tr = find_cycle(graph, "tr");

    std::println("  part 2: {}", std::lcm(xm, std::lcm(nh, std::lcm(dr, tr))));

}