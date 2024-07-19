#include "../util/util.h"
#include "y2021.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_set>
#include <iterator>
#include <Eigen/Dense>
#include <boost/functional/hash.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {
    using matrix = Eigen::Matrix<int, 3, 3>;
    using vec3 = Eigen::Matrix<int, 3, 1>;

    using vec3_vector = std::vector<vec3>;

    struct vec3_hash_t {
        std::size_t operator()(const vec3& v) const {
            size_t seed = 0;
            boost::hash_combine(seed, v[0]);
            boost::hash_combine(seed, v[1]);
            boost::hash_combine(seed, v[2]);
            return seed;
        }
    };

    struct vec3_equal_t {
        bool operator()(const vec3& v1, const vec3& v2) const {
            return v1[0] == v2[0] && v1[1] == v2[1] && v1[2] == v2[2];
        }
    };

    using vec3_set = std::unordered_set<vec3, vec3_hash_t, vec3_equal_t>;

    int num_common_points(const vec3_set& data_set, const vec3_vector& test) {
        int count = 0;
        for (const auto& v : test) {
            count += (data_set.find(v) != data_set.end()) ? 1 : 0;
        }
        return count;
    }

    std::vector<matrix> rotations_of_the_cube() {

        static auto three_combos_with_rep_of_1_and_neg_1 = []() {
            std::vector<std::tuple<int, int, int>> combos(8);
            auto make_unit = [](int v) {return 2 * v - 1; };
            for (int i = 0; i < 8; i++) {
                combos[i] = { make_unit((i & (1 << 2)) >> 2),  make_unit((i & (1 << 1)) >> 1), make_unit(i & 1) };
            }
            return combos;
            };

        static auto gen_matrices = [](const std::tuple<int, int, int>& tup)->std::vector<matrix> {
            auto [u1, u2, u3] = tup;
            matrix m1, m2, m3, m4, m5, m6;

            m1 << u1, 0, 0,
                0, u2, 0,
                0, 0, u3;

            m2 << 0, u1, 0,
                0, 0, u2,
                u3, 0, 0;

            m3 << 0, 0, u1,
                u2, 0, 0,
                0, u3, 0;

            m4 << u1, 0, 0,
                0, 0, u2,
                0, u3, 0;

            m5 << 0, 0, u1,
                0, u2, 0,
                u3, 0, 0;

            m6 << 0, u1, 0,
                u2, 0, 0,
                0, 0, u3;

            return { m1,m2,m3,m4,m5,m6 };
            };

        static auto combos = three_combos_with_rep_of_1_and_neg_1();
        std::vector<matrix> rotations;
        for (const auto& combo : combos) {
            auto symmetries_of_the_cube = gen_matrices(combo);
            for (const auto& mat : symmetries_of_the_cube) {
                if (mat.determinant() == 1) {
                    rotations.push_back(mat);
                }
            }
        }
        return rotations;
    }

    vec3 string_to_vec3(const std::string& str) {
        auto p = aoc::split(str, ',');
        return vec3(std::stoi(p[0]), std::stoi(p[1]), std::stoi(p[2]));
    }

    std::vector<vec3_vector> parse_scanner_data(const std::vector<std::string>& input) {
        std::vector<std::string> lines;
        lines.reserve(input.size());
        std::copy_if(input.begin(), input.end(), std::back_inserter(lines),
            [](const auto& str) {
                return str.empty() || str.substr(0, 3) != "---";
            }
        );

        std::vector<vec3_vector> all_scanner_data;
        std::vector<std::string>::iterator iter = lines.begin();
        while (iter != lines.end()) {
            vec3_vector scanner_data;
            auto j = std::find(iter, lines.end(), "");
            for (auto i = iter; i < j; ++i) {
                scanner_data.push_back(string_to_vec3(*i));
            }
            all_scanner_data.emplace_back(std::move(scanner_data));
            iter = (j != lines.end()) ? j + 1 : j;
        }

        return all_scanner_data;
    }

    vec3_vector translate(const vec3_vector& vec, const vec3& translation) {
        vec3_vector translated(vec.size());
        std::transform(vec.begin(), vec.end(), translated.begin(),
            [&translation](const vec3& v) { return v + translation; }
        );
        return translated;
    }

    std::tuple<vec3_vector, vec3> finding_matching_orientation_and_offset(const vec3_set& known_pts, const vec3_vector& data) {
        static auto rotations = rotations_of_the_cube();
        for (const auto& rotation : rotations) {

            vec3_vector rotated_data(data.size());
            std::transform(data.begin(), data.end(), rotated_data.begin(),
                [&rotation](const vec3& v) {return rotation * v; }
            );

            for (const auto& known_pt : known_pts) {
                for (const auto& test_pt : rotated_data) {
                    auto translation = known_pt - test_pt;
                    auto rotated_and_translated = translate(rotated_data, translation);
                    if (num_common_points(known_pts, rotated_and_translated) >= 12) {
                        return { std::move(rotated_and_translated), std::move(translation) };
                    }
                }
            }
        }
        return {};
    }

    std::tuple<vec3_vector, vec3_vector> find_beacons_and_scanners(const std::vector<vec3_vector>& scanner_readings) {
        vec3_vector scanners = { {0,0,0} };
        std::vector<vec3_vector> work_set(scanner_readings.size() - 1);
        vec3_set beacon_set(scanner_readings[0].begin(), scanner_readings[0].end());
        std::copy(std::next(scanner_readings.begin()), scanner_readings.end(), work_set.begin());

        while (!work_set.empty()) {
            std::vector<vec3_vector>::iterator i;
            vec3_vector match;
            vec3 origin;
            for (i = work_set.begin(); i != work_set.end(); ++i) {
                std::tie(match, origin) = finding_matching_orientation_and_offset(beacon_set, *i);
                if (!match.empty()) {
                    break;
                }
            }
            if (match.empty()) {
                throw std::runtime_error("the beacons dont line up");
            }
            std::copy(match.begin(), match.end(), std::inserter(beacon_set, beacon_set.end()));
            work_set.erase(i);
            scanners.push_back(origin);
        }
        vec3_vector beacons(beacon_set.begin(), beacon_set.end());
        return { std::move(beacons), std::move(scanners) };
    }

    int manhattan_distance(const vec3& v1, const vec3& v2) {
        return std::abs(v1[0] - v2[0]) + std::abs(v1[1] - v2[1]) + std::abs(v1[2] - v2[2]);
    }

    int farthest_apart_scanner_distance(const vec3_vector& scanners) {
        int max_distance = 0;
        for (int i = 0; i < scanners.size() - 1; ++i) {
            for (int j = i; j < scanners.size(); j++) {
                auto dist = manhattan_distance(scanners[i], scanners[j]);
                max_distance = (dist > max_distance) ? dist : max_distance;
            }
        }
        return max_distance;
    }
}

void aoc::y2021::day_19(const std::string& title) {
    auto all_scanner_data = parse_scanner_data(
        aoc::file_to_string_vector(aoc::input_path(2021, 19))
    );
    auto [beacons, scanners] = find_beacons_and_scanners(all_scanner_data);

    std::println("--- Day 19: {} ---", title);
    std::println("  part 1: {}", beacons.size());
    std::println("  part 2: {}", farthest_apart_scanner_distance(scanners));
}