#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <glm/ext.hpp>
#include <limits>
#include <chrono>

namespace {
    using namespace std::chrono_literals;
    using nanoseconds  = std::chrono::nanoseconds;
    using microseconds = std::chrono::microseconds;
    using milliseconds = std::chrono::milliseconds;
    using seconds      = std::chrono::seconds;
    using minutes      = std::chrono::minutes;
    using hours        = std::chrono::hours;
    using days         = std::chrono::days;
    using weeks        = std::chrono::weeks;
    using years        = std::chrono::years;
    using months       = std::chrono::months;
};

constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();
constexpr double MAX_FLOAT  = std::numeric_limits<float>::max();
constexpr double MIN_FLOAT  = std::numeric_limits<float>::lowest();

constexpr double PI = glm::pi<double>();
constexpr double TWO_PI = 2.0 * PI;
constexpr double HALF_PI = glm::half_pi<double>();

#endif