
#include "../util/util.h"
#include "y2018.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using digraph = std::unordered_map<std::string, std::vector<std::string>>;

    std::tuple<std::string, std::string> parse_edge(const std::string& str) {
        auto words = aoc::split(str, ' ');
        return { words[1],words[7] };
    }

    digraph parse_digraph(const std::vector<std::string>& inp) {
        digraph g;
        for (const auto& [u, v] : inp | rv::transform(parse_edge)) {
            if (!g.contains(v)) {
                g[v] = {};
            }
            g[u].push_back(v);
        }
        return g;
    }

    std::unordered_map<std::string, int> vertex_in_degrees(const digraph& g) {
        std::unordered_map<std::string, int> in_degree;
        for (const auto& [u, adj_list] : g) {
            in_degree[u] = 0;
        }
        for (const auto& [u, adj_list] : g) {
            for (const auto& v : adj_list) {
                in_degree[v]++;
            }
        }
        return in_degree;
    }

    std::string lexographically_smallest_topological_order(const digraph& g) {
        std::unordered_map<std::string, int> in_degrees = vertex_in_degrees(g);
        std::set<std::string> priority_queue;

        for (const auto& [u, in_degree] : in_degrees) {
            if (in_degree == 0) {
                priority_queue.insert(u);
            }
        }

        std::stringstream topo_order;
        while (!priority_queue.empty()) {
            auto current = priority_queue.extract(priority_queue.begin()).value();
            topo_order << current;
            for (auto adj : g.at(current)) {
                if (--in_degrees[adj] == 0) {
                    priority_queue.insert(adj);
                }
            }
        }

        return topo_order.str();
    }

    class worker_pool {
    
        struct task {
            std::string label;
            int time_remaining;
        };
        int max_workers_;
        std::vector<task> tasks_;
    public:
        worker_pool(int n) : max_workers_(n)
        {}

        void insert_task(const std::string& task_id, int duration) {
            if (num_free_workers() == 0) {
                throw std::runtime_error("no workers available");
            }
            tasks_.emplace_back(task_id, duration);
        }

        int num_free_workers() const {
            return max_workers_ - tasks_.size();
        }

        bool has_free_workers() const {
            return tasks_.size() < max_workers_;
        }

        bool empty() const {
            return tasks_.empty();
        }

        std::tuple<int, std::vector<std::string>> run_until_next_event() {
            auto time_until_next_event = r::min(
                tasks_ | rv::transform([](auto&& t) {return t.time_remaining; })
            );

            std::vector<std::string> completed;
            std::vector<task> new_tasks;
            for (const auto& t : tasks_) {
                auto time_remaining = t.time_remaining - time_until_next_event;
                if (time_remaining == 0) {
                    completed.push_back(t.label);
                } else {
                    new_tasks.emplace_back(t.label, time_remaining);
                }
            }
            tasks_ = new_tasks;
            return { time_until_next_event, completed };
            
        }

    };

    int min_time(const digraph& g, int num_workers, int base_task_time) {
        std::unordered_map<std::string, int> in_degrees = vertex_in_degrees(g);
        std::set<std::string> task_queue;
        worker_pool work_pool(num_workers);

        auto task_duration = [base_task_time](const std::string& lbl)->int {
            return base_task_time + (lbl.front() - 'A' + 1);
        };

        for (const auto& [u, in_degree] : in_degrees) {
            if (in_degree == 0) {
                task_queue.insert(u);
            }
        }

        int elapsed_time = 0;
        while (!task_queue.empty() || !work_pool.empty()) {
            
            while (work_pool.has_free_workers() && !task_queue.empty()) {
                auto task = task_queue.extract(task_queue.begin()).value();
                work_pool.insert_task(task, task_duration(task));
            }

            auto [elapsed, completed] = work_pool.run_until_next_event();
            elapsed_time += elapsed;
            for (auto current : completed) {
                for (auto adj : g.at(current)) {
                    if (--in_degrees[adj] == 0) {
                        task_queue.insert(adj);
                    }
                }
            }
        }

        return elapsed_time;
    }
}

void aoc::y2018::day_07(const std::string& title) {

    auto graph = parse_digraph(
        aoc::file_to_string_vector(
            aoc::input_path(2018, 7)
        )
    );

    std::println("--- Day 7: {} ---", title);
    std::println("  part 1: {}", 
        lexographically_smallest_topological_order(graph)
    );
    std::println("  part 2: {}", min_time(graph, 5, 60));
    
}
