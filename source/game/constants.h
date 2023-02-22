#ifndef GAME_CONSTANTS_H_
#define GAME_CONSTANTS_H_

#include "../constants.h"
#include <chrono>

using namespace std::chrono_literals;
using ms = std::chrono::milliseconds;

constexpr int       ASTEROID_CHUNK_PADDING = 1;
constexpr glm::vec2 ASTEROID_CHUNK_SIZE    = glm::vec2(50.f);
constexpr double    ASTEROID_DENSITY       = 0.000005;
constexpr double    ASTEROID_AVG_SPEED     = 1;
constexpr double    ASTEROID_SPEED_SIGMA   = 5;
constexpr double    ASTEROID_MAX_SPIN      = 0.05;
constexpr double    ASTEROID_SPEED_DECAY   = 0.002;

constexpr double    DRONE_SIGHT_RADIUS     = 500;
constexpr double    DRONE_TURN_RATE        = 0.05;
constexpr double    DRONE_BEAM_RANGE       = 20;
constexpr double    DRONE_MAX_SPEED        = 10;
constexpr ms        DRONE_WANDER_DURATION  = 500ms;
constexpr ms        DRONE_PURSUE_DURATION  = 500ms;
#endif