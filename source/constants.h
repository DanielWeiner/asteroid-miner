#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <glm/ext.hpp>
#include <limits>

constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();
constexpr double MAX_FLOAT  = std::numeric_limits<float>::max();
constexpr double MIN_FLOAT  = std::numeric_limits<float>::lowest();

constexpr double PI = glm::pi<double>();
constexpr double TWO_PI = 2.0 * PI;
constexpr double HALF_PI = glm::half_pi<double>();

#endif