#include "days.h"
#include "../util.h"
#include <print>
#include <ranges>
#include <numeric>
#include <algorithm>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {
    struct vec3 {
        int64_t x;
        int64_t y;
        int64_t z;
    };

    vec3 operator+(const vec3& lhs, const vec3& rhs) {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    vec3 operator*(int64_t lhs, const vec3& rhs) {
        return { lhs * rhs.x, lhs * rhs.y, lhs * rhs.z};
    }

    bool operator==(const vec3& lhs, const vec3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    struct particle {
        vec3 pos;
        vec3 vel;
    };

    vec3 position_at_time(const particle& p, int64_t t) {
        return p.pos + t * p.vel;
    }

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

    struct collision {
        vec3 where;
        int64_t when;
    };

    std::optional<int64_t> find_collision(int64_t pa, int64_t pb, int64_t va, int64_t vb) {
        auto pb_minus_pa = pb - pa;
        auto va_minus_vb = va - vb;

        if (va_minus_vb == 0) {
            return {};
        }

        //we only want collisions with integer coordinates.
        if (pb_minus_pa % va_minus_vb != 0) {
            return {};
        }
        return pb_minus_pa / va_minus_vb;
    }

    std::optional<collision> find_collision(const particle& a, const particle& b) {
        auto t = find_collision(a.pos.x, b.pos.x, a.vel.x, b.vel.x);
        if (!t) {
            t = find_collision(a.pos.y, b.pos.y, a.vel.y, b.vel.y);
        }
        if (!t) {
            t = find_collision(a.pos.z, b.pos.z, a.vel.z, b.vel.z);
        }

        if (!t) {
            return {};
        }
        
        //we only want collisions in the future
        if (*t < 0) {
            return {};
        }

        auto a_pos = position_at_time(a, *t);
        auto b_pos = position_at_time(b, *t);
        if (a_pos == b_pos) {
            return collision{ a_pos, *t };
        }
        return {};
    }

    std::vector<collision> find_collisions(const std::vector<particle>& particles) {
        int n = static_cast<int>(particles.size());
        std::vector<collision> collisions;
        for (int i = 0; i < n - 1; ++i) {
            for (int j = i + 1; j < n; ++j) {
                auto a = particles[i];
                auto b = particles[j];
                auto collision = find_collision(a, b);
                if (collision) {
                    collisions.push_back(*collision);
                }
            }
        }
        return collisions;
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
    auto collisions = find_collisions(particles);
    std::println("{}", collisions.size());
}