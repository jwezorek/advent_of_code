#include "y2023.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>
#include <boost/multiprecision/cpp_bin_float.hpp>

namespace r = std::ranges;
namespace rv = std::ranges::views;
namespace bm = boost::multiprecision;

namespace {
    struct vec3 {
        int64_t x;
        int64_t y;
        int64_t z;
    };

    vec3 operator+(const vec3& lhs, const vec3& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    vec3 operator-(const vec3& lhs, const vec3& rhs) {
        return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
    }

    vec3 operator*(int64_t lhs, const vec3& rhs) {
        return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z };
    }

    bool operator==(const vec3& lhs, const vec3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    struct particle {
        vec3 pos;
        vec3 vel;
    };

    struct vec2 {
        double x;
        double y;

        vec2(int64_t ix, int64_t iy) :
            x(ix), y(iy)
        {}

        vec2(double x, double y) :
            x(x), y(y)
        {}
    };

    vec2 operator+(const vec2& lhs, const vec2& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    vec2 operator*(double lhs, const vec2& rhs) {
        return { lhs * rhs.x, lhs * rhs.y };
    }

    particle str_to_particle(const std::string& inp) {
        auto v = aoc::extract_numbers_int64(inp);
        return {
            {v[0],v[1],v[2]},
            {v[3],v[4],v[5]}
        };
    }

    std::string particle_to_str(const particle& p) {
        return std::format("[{} {} {} @ {} {} {}]",
            p.pos.x, p.pos.y, p.pos.z,
            p.vel.x, p.vel.y, p.vel.z
        );
    }

    std::optional<vec2> ray_intersection(const vec2& as, const vec2& ad, const vec2& bs, const vec2& bd)
    {
        auto dx = bs.x - as.x;
        auto dy = bs.y - as.y;
        auto det = bd.x * ad.y - bd.y * ad.x;
        if (det != 0) { // near parallel line will yield noisy results
            double u = (dy * bd.x - dx * bd.y) / (double)det;
            double v = (dy * ad.x - dx * ad.y) / (double)det;
            if (u >= 0 && v >= 0) {
                return as + u * ad;
            }
        }
        return {};
    }

    int count_2d_intersections(const std::vector<particle>& particles, double min_dim, double max_dim) {
        int n = static_cast<int>(particles.size());
        int count = 0;
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                auto a = particles[i];
                auto b = particles[j];
                auto intersection = ray_intersection(
                    { a.pos.x, a.pos.y }, { a.vel.x, a.vel.y },
                    { b.pos.x, b.pos.y }, { b.vel.x, b.vel.y }
                );
                if (intersection) {
                    auto [x, y] = *intersection;
                    if (x >= min_dim && x <= max_dim && y >= min_dim && y <= max_dim) {
                        ++count;
                    }
                }
            }
        }
        return count;
    }

    std::optional<vec3> line_line_intersection( vec3 p1, vec3 p2, vec3 p3, vec3 p4) {
        using number = boost::multiprecision::cpp_bin_float_quad;

        vec3 p13, p43, p21;
        number d1343, d4321, d1321, d4343, d2121;
        number numer, denom;

        p13.x = p1.x - p3.x;
        p13.y = p1.y - p3.y;
        p13.z = p1.z - p3.z;
        p43.x = p4.x - p3.x;
        p43.y = p4.y - p3.y;
        p43.z = p4.z - p3.z;
        if (p43.x == 0 && p43.y == 0 && p43.z == 0)
            return {};

        p21.x = p2.x - p1.x;
        p21.y = p2.y - p1.y;
        p21.z = p2.z - p1.z;
        if (p21.x == 0 && p21.y == 0 && p21.z == 0)
            return {};

        d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
        d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
        d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
        d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
        d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

        denom = d2121 * d4343 - d4321 * d4321;
        if (denom == 0)
            return {};
        numer = d1343 * d4321 - d1321 * d4343;

        number mua = static_cast<number>(numer) / static_cast<number>(denom);
        number mub = static_cast<number>(d1343 + d4321 * mua) / static_cast<number>(d4343);

        auto ax = bm::round(static_cast<number>(p1.x) + mua * static_cast<number>(p21.x));
        auto ay = bm::round(static_cast<number>(p1.y) + mua * static_cast<number>(p21.y));
        auto az = bm::round(static_cast<number>(p1.z) + mua * static_cast<number>(p21.z));
        auto bx = bm::round(static_cast<number>(p3.x) + mub * static_cast<number>(p43.x));
        auto by = bm::round(static_cast<number>(p3.y) + mub * static_cast<number>(p43.y));
        auto bz = bm::round(static_cast<number>(p3.z) + mub * static_cast<number>(p43.z));

        vec3 a = { static_cast<int64_t>(ax), static_cast<int64_t>(ay), static_cast<int64_t>(az) };
        vec3 b = { static_cast<int64_t>(bx), static_cast<int64_t>(by), static_cast<int64_t>(bz) };

        if (a == b) {
            return a;
        }

        return {};
    }

    std::optional<vec3> find_velocity(const std::vector<particle>& particles, int64_t range) {
        for (int64_t y_vel = -range; y_vel <= range; ++y_vel) {
            for (int64_t x_vel = -range; x_vel <= range; ++x_vel) {
                for (int64_t z_vel = -range; z_vel <= range; ++z_vel) {
                    vec3 delta = { x_vel, y_vel, z_vel };

                    std::optional<vec3> pt = {};
                    int count = 0;
                    for (int i = 1; i <= 5; ++i) {
                        auto collision_pt = line_line_intersection(particles[0].pos, particles[0].pos + particles[0].vel + delta, 
                            particles[i].pos, particles[i].pos + particles[i].vel + delta);
                        if (!collision_pt) {
                            count = 0;
                            break;
                        }
                        if (!pt) {
                            pt = *collision_pt;
                        }
                        if (*collision_pt == pt) {
                            count++;
                        }
                    }
                    if (count == 5) {
                        return delta;
                    }
                }
            }
        }
        return {};
    }

}

/*------------------------------------------------------------------------------------------------*/

void aoc::y2023::day_24(const std::string& title) {

    auto particles = aoc::file_to_string_vector(aoc::input_path(2023, 24)) |
        rv::transform(str_to_particle) | r::to<std::vector<particle>>();

    std::println("--- Day 24: {0} ---\n", title);
    std::println("  part 1: {}", 
        count_2d_intersections( particles, 200000000000000.0, 400000000000000.0)
    );

    auto vel = find_velocity(particles, 250);
    auto pt = line_line_intersection(particles[0].pos, particles[0].pos + particles[0].vel + *vel,
        particles[1].pos, particles[1].pos + particles[1].vel + *vel);

    std::println("  part 2: {}", pt->x + pt->y + pt->z);

}