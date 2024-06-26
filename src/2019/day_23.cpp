#include "../util.h"
#include "intcode.h"
#include "y2019.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <queue>
#include <unordered_set>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct packet {
        int64_t x;
        int64_t y;
    };

    class network_computer;
    using network = std::vector<network_computer>;

    class network_computer {

        aoc::intcode_computer nic_;
        std::queue<packet> queue_;

        std::optional<packet> dequeue() {
            if (queue_.empty()) {
                return {};
            }
            auto packet = queue_.front();
            queue_.pop();

            return packet;
        }

    public:
        network_computer(const std::vector<int64_t>& nic, int64_t id) :
                nic_(nic) {
            auto event = nic_.run_until_event(id);
            if (event != aoc::received_input) {
                throw std::runtime_error( "???" );
            }
        }

        void enqueue(const packet& p) {
            queue_.push(p);
        }

        void do_work(network& nw, std::function<void(const packet&)> on_send_to_nat) {
            auto next_event = nic_.run_until_event();
            if (next_event == aoc::awaiting_input) {
                auto queued_packet = dequeue();
                if (queued_packet) {
                    nic_.run_until_event(queued_packet->x);
                    nic_.run_until_event(queued_packet->y);
                } else {
                    nic_.run_until_event(-1);
                }
                return;
            } else if (next_event == aoc::generated_output) {
                int64_t address = nic_.output();
                nic_.run_until_event();
                int64_t x = nic_.output();
                nic_.run_until_event();
                int64_t y = nic_.output();
                packet p{ x,y };
                if (address >= 0 && address < nw.size()) {
                    nw[address].enqueue(p);
                } else if (address == 255) {
                    on_send_to_nat(p);
                }
                return;
            }
            throw std::runtime_error("network computer in bad state");
        }

        bool is_idle() const {
            if (!queue_.empty()) {
                return false;
            }
            auto clone = nic_;
            return (clone.run_until_event() == aoc::awaiting_input);
        }
    };

    network make_network(const std::vector<int64_t>& nic, int sz) {
        return rv::iota(0, sz) |
            rv::transform(
                [&](auto id) {
                    return network_computer(nic, id);
                }
            ) | r::to<std::vector>();
    }

    class nat_computer {
        std::optional<packet> packet_;
        std::optional<int64_t> last_y_;
        std::optional<int64_t> first_y_dupe_;
    public:
        nat_computer() 
        {}

        void receive_packet(const packet& p) {
            packet_ = p;
        }

        void do_work(network& nw) {
            for (const auto& comp : nw) {
                if (!comp.is_idle()) {
                    return;
                }
            }
            nw[0].enqueue(*packet_);
            if (last_y_ && *last_y_ == packet_->y) {
                first_y_dupe_ = *last_y_;
            }
            last_y_ = packet_->y;
        }

        std::optional<int64_t> result() const {
            return first_y_dupe_;
        }
    };

    void do_work(
            const network& nw, 
            std::function<bool()> process_complete,
            std::function<void(const packet&)> on_send_to_nat,
            std::function<void(network&)> on_iteration) {
        auto network = nw;
        while (! process_complete() ) {
            for (auto& comp : network) {
                comp.do_work(network, on_send_to_nat);
                if (process_complete()) {
                    break;
                }
            }
            on_iteration(network);
        }
    }

    int64_t do_part_1(const network& network) {
        std::optional<int64_t> result;
        do_work(
            network,
            [&]()->bool { return result.has_value(); },
            [&](const packet& p) {
                result = p.y;
            },
            [](::network&) {}
        );
        return *result;
    }

    int64_t do_part_2(const network& network) {
        nat_computer nat;
        do_work(
            network,
            [&]()->bool { 
                return nat.result().has_value(); 
            },
            [&](const packet& p) {
                nat.receive_packet(p);
            },
            [&](::network& nw) { 
                nat.do_work(nw); 
            }
        );
        return nat.result().value();
    }
}

void aoc::y2019::day_23(const std::string& title) {

    auto nic = split(
            aoc::file_to_string(aoc::input_path(2019, 23)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return aoc::string_to_int64(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 23: {} ---", title);
    std::println("  part 1: {}",
        do_part_1(make_network(nic, 50))
    );
    std::println("  part 2: {}",
        do_part_2(make_network(nic, 50))
    );
}