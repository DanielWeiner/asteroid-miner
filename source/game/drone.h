#ifndef DRONE_H_
#define DRONE_H_

#include "constants.h"
#include "asteroid.h"
#include "../sprite.h"
#include <glm/glm.hpp>
#include <memory>
#include <span>

#include <chrono>
using ms = std::chrono::milliseconds;

class OuterSpaceScene;
class LineRenderer;

class Drone {
public:
    enum class DroneState {
        INITIAL,
        SEARCHING,
        WANDERING,
        PURSUING,
        BEAMING,
        MINING
    };
    
    Drone(std::unique_ptr<Sprite>&& sprite);
    void setSpeed(glm::vec2 speed);
    void moveTo(glm::vec2 position);
    void step(OuterSpaceScene& scene);

    void render(LineRenderer& lineRenderer);
    void calculateEdgeTangents();

    glm::vec2 getSpeed();
    glm::vec2 getPosition();
    glm::vec2 getSize();

private:
    static constexpr double DRONE_MAX_SPEED_2 = DRONE_MAX_SPEED * DRONE_MAX_SPEED;
    static constexpr double DRONE_SIGHT_RADIUS_2 = DRONE_SIGHT_RADIUS * DRONE_SIGHT_RADIUS;
    static constexpr double DRONE_BEAM_RANGE_2 = DRONE_BEAM_RANGE * DRONE_BEAM_RANGE;

    double                    _maxSpeed = DRONE_MAX_SPEED;
    double                    _maxSpeed2 = DRONE_MAX_SPEED_2;
    double                    _turnRate = DRONE_TURN_RATE;
    double                    _sightRadius = DRONE_SIGHT_RADIUS;
    double                    _sightRadius2 = DRONE_SIGHT_RADIUS_2;
    double                    _beamRange = DRONE_BEAM_RANGE;
    double                    _beamRange2 = DRONE_BEAM_RANGE_2;
    ms                        _wanderDuration = DRONE_WANDER_DURATION;
    ms                        _pursueDuration = DRONE_PURSUE_DURATION;

    long long                 _pursueStart = 0ll;
    long long                 _wanderStart = 0ll;

    std::shared_ptr<Asteroid> _targetAsteroid;
    std::unique_ptr<Sprite>   _sprite;
    glm::vec2                 _speed = glm::vec2(0);
    glm::vec2                 _acceleration = glm::vec2(0);

    DroneState                _state = DroneState::INITIAL;

    void _accelerateToward(glm::vec2 point);

    void _rotateDirection();
    void _searchForAsteroid(OuterSpaceScene& scene);
    void _wander();

    void _pursueAsteroid();
    void _beamAsteroid();
    void _mineAsteroid();
};

#endif