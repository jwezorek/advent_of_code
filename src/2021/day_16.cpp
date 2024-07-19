#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <numeric>
#include <variant>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    using byte = unsigned char;

    std::vector<byte> hex_string_to_binary(const std::string& str) {
        auto hex = (str.size() % 2 == 0) ? str : str + '0';

        int n = hex.size() / 2;
        std::vector<byte> bytes(n);
        for (int i = 0; i < n; ++i) {
            bytes[i] = std::stoi(hex.substr(i * 2, 2), 0, 16);
        }

        return bytes;
    }

    class bit_iterator {
    private:
        std::vector<byte>::const_iterator iter_;
        int bit_;
    public:
        bit_iterator(std::vector<byte>::const_iterator iter) :
            iter_(iter),
            bit_(7)
        {}

        bit_iterator& operator++() {
            bit_--;
            if (bit_ == -1) {
                bit_ = 7;
                iter_++;
            }
            return *this;
        }

        bit_iterator& operator+=(int n) {
            for (int i = 0; i < n; ++i) {
                ++(*this);
            }
            return *this;
        }

        // postfix increment...
        bit_iterator operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
        }

        int operator* ()
        {
            auto byte = *iter_;
            return static_cast<unsigned int>((byte & (1 << bit_)) >> bit_);
        }

        bool operator!=(const bit_iterator& o) {
            if (iter_ != o.iter_) {
                return true;
            }
            return bit_ != o.bit_;
        }

        int get_bits(int n) {
            auto& self = *this;
            int out = 0;
            for (int i = 0; i < n; ++i) {
                auto bit = *(self++);
                out += bit << (n - i - 1);
            }
            return out;
        }

        int peek(int n) {
            auto self = *this;
            int val = get_bits(n);
            *this = self;
            return val;
        }

    };

    bit_iterator operator+(const bit_iterator& iter, int n) {
        bit_iterator i = iter;
        i += n;
        return i;
    }

    struct literal_packet {
        int version;
        int type;
        uint64_t data;
    };

    struct op_packet;
    using packet_t = std::variant<literal_packet, std::shared_ptr<op_packet>>;
    struct op_packet {
        int version;
        int type;
        std::vector<packet_t> subpackets;
    };

    uint64_t hex_digits_to_int(const std::vector<byte> digits) {
        if (digits.size() > 16) {
            throw std::runtime_error("literal value too large");
        }
        uint64_t sixteens_place = 1;
        uint64_t val = 0;
        for (auto i = digits.rbegin(); i != digits.rend(); ++i) {
            val += sixteens_place * (*i);
            sixteens_place *= 16;
        }
        return val;
    }

    std::optional<literal_packet> parse_literal_packet(bit_iterator& i) {
        auto old = i;
        literal_packet packet;
        packet.version = i.get_bits(3);
        packet.type = i.get_bits(3);

        if (packet.type != 4) {
            i = old;
            return std::nullopt;
        }
        bool done = false;
        std::vector<byte> data;
        while (!done) {
            done = !(*(i++));
            data.push_back(i.get_bits(4));
        }
        packet.data = hex_digits_to_int(data);
        return packet;
    }

    std::vector<packet_t> parse_packets(bit_iterator& i, bit_iterator j);
    std::vector<packet_t> parse_n_packets(bit_iterator& i, int n);

    std::optional<op_packet> parse_op_packet(bit_iterator& i) {
        auto old = i;
        op_packet packet;
        packet.version = i.get_bits(3);
        packet.type = i.get_bits(3);
        if (packet.type == 4) {
            i = old;
            return std::nullopt;
        }
        int length_type = *(i++);
        if (length_type == 0) { // length is the number of bits in the subpackets...
            int num_bits = i.get_bits(15);
            packet.subpackets = parse_packets(i, i + num_bits);
        }
        else {  // length is the number of subpackets...
            int num_packets = i.get_bits(11);
            packet.subpackets = parse_n_packets(i, num_packets);
        }
        return packet;
    }

    packet_t parse_packet(bit_iterator& i) {
        auto maybe_literal = parse_literal_packet(i);
        if (maybe_literal) {
            return *maybe_literal;
        }
        auto maybe_op = parse_op_packet(i);
        if (!maybe_op) {
            throw std::runtime_error("bad packet");
        }
        return std::make_shared<op_packet>(*maybe_op);
    }

    std::vector<packet_t> parse_packets(bit_iterator& i, bit_iterator j) {
        std::vector<packet_t> packets;
        while (i != j) {
            packets.push_back(parse_packet(i));
        }
        return packets;
    }

    std::vector<packet_t> parse_n_packets(bit_iterator& i, int n) {
        std::vector<packet_t> packets(n);
        for (int j = 0; j < n; ++j) {
            packets[j] = parse_packet(i);
        }
        return packets;
    }

    using visit_lit_fn = std::function<uint64_t(const literal_packet&)>;
    using visit_op_fn = std::function<uint64_t(std::shared_ptr<op_packet>)>;

    struct packet_visitor {
        visit_lit_fn visit_lit_;
        visit_op_fn visit_op_;

        uint64_t operator()(const literal_packet& p) {
            return visit_lit_(p);
        }

        uint64_t operator()(std::shared_ptr<op_packet> p) {
            return visit_op_(p);
        }
    };

    int sum_of_version_numbers(const packet_t& packet) {
        auto visitor = packet_visitor{
            [](const  literal_packet& p) {return p.version; },
            [](std::shared_ptr<op_packet> p) {
                int sum = 0;
                sum = p->version;
                for (const auto& child : p->subpackets) {
                    sum += sum_of_version_numbers(child);
                }
                return sum;
            }
        };
        return std::visit(visitor, packet);
    }

    uint64_t evaluate_packet(const packet_t& packet) {
        static std::array<std::function<uint64_t(const std::vector<uint64_t>&)>, 8> ops = {
            [](const auto& args) {return std::accumulate(args.begin(), args.end(), 0ull); },
            [](const auto& args) {return std::accumulate(args.begin(), args.end(), 1ull, std::multiplies<uint64_t>()); },
            [](const auto& args) {return *std::min_element(args.begin(), args.end()); },
            [](const auto& args) {return *std::max_element(args.begin(), args.end()); },
            [](const auto& args)->uint64_t { throw std::runtime_error("bad op code"); } ,
            [](const auto& args) {return args[0] > args[1] ? 1 : 0; },
            [](const auto& args) {return args[0] < args[1] ? 1 : 0; },
            [](const auto& args) {return args[0] == args[1] ? 1 : 0; }
        };
        auto visitor = packet_visitor{
            [](const  literal_packet& p) {return p.data; },
            [](std::shared_ptr<op_packet> p) {
                std::vector<uint64_t> args(p->subpackets.size());
                std::transform(p->subpackets.begin(), p->subpackets.end(), args.begin(), evaluate_packet);
                return ops[p->type](args);
            }
        };
        return std::visit(visitor, packet);
    }
}

void aoc::y2021::day_16(const std::string& title) {

    auto bytes = hex_string_to_binary(file_to_string(aoc::input_path(2021, 16)));
    bit_iterator iter = bytes.begin();
    auto packet = parse_packet(iter);

    std::println("--- Day 16: {} ---", title);
    std::println("  part 1: {}", sum_of_version_numbers(packet));
    std::println("  part 2: {}", evaluate_packet(packet));
}