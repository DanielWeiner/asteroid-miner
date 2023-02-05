#include "drone.h"
#include "../time.h"
#include "../random.h"
#include "outerSpaceScene.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/norm.hpp>
#include "constants.h"
#include <iostream>

Drone::Drone(std::unique_ptr<Sprite> &&sprite) : _sprite(std::move(sprite)) {}

void Drone::setSpeed(glm::vec2 speed) { _speed = speed; }

void Drone::moveTo(glm::vec2 position) 
{
    _sprite->moveTo(position);
}

void Drone::step(OuterSpaceScene& scene) 
{
    _angle = glm::atan(_speed.y, _speed.x);

    switch (_state) {
    case DroneState::SEARCHING:
        _searchForAsteroid(scene);
        break;
    case DroneState::PURSUING:
        _pursueAsteroid();
        break;
    case DroneState::WANDERING:
        _wander();
        break;
    case DroneState::BEAMING:
        _beamAsteroid();
        break;
    case DroneState::MINING:
        _mineAsteroid();
        break;
    }

    auto unit = glm::vec2(glm::cos(_angle), glm::sin(_angle));
    auto velocity = glm::length(_speed);

    _speed = unit * (velocity + (float)_acceleration);

    if (_accelerationRate != 0 && glm::length2(_speed) > _maxSpeed2) {
        _speed = unit * velocity * (float)_maxSpeed;
    }

    _sprite->move(_speed);

    _rotateDirection();

    _sprite->update();
}

glm::vec2 Drone::getSpeed()
{
    return _speed;
}

glm::vec2 Drone::getPosition()
{
    return _sprite->getPosition();
}

glm::vec2 Drone::getSize()
{
    return _sprite->getSize();
}

void Drone::_accelerateDecelerate(
    double maxSpeed,
    long long distance,
    long long progress
) {
    _acceleration = -2 * (maxSpeed * (progress-distance/2)) / (distance * distance);
}

void Drone::_rotateDirection() {
    _sprite->rotateTo(HALF_PI + glm::atan(_speed.y, _speed.x));
}

void Drone::_searchForAsteroid(OuterSpaceScene& scene) 
{
    std::shared_ptr<Asteroid> closestAsteroid;
    float shortestDistance = MAX_FLOAT;
    for (auto asteroid : scene.asteroids()) {
        auto distance = glm::distance(asteroid->getPosition(), _sprite->getPosition());
        if (distance < DRONE_SIGHT_RADIUS) {
            shortestDistance = glm::min(distance, shortestDistance);
            if (shortestDistance == distance) {
                closestAsteroid = asteroid;
            }
        }
    }

    if (closestAsteroid) {
        _targetAsteroid = closestAsteroid;
        _state = DroneState::PURSUING;
    } else {
        _state = DroneState::WANDERING;
    }
}

void Drone::_wander() 
{
    if (_wanderStart == 0ll) {
        _wanderStart = Time::timestamp();
        _angle = Random::uniform() * TWO_PI;
    }
    
    _accelerateDecelerate(_maxSpeed, Time::milliseconds(_wanderDuration), Time::timestamp() - _wanderStart); 

    if (Time::milliseconds(_wanderDuration) + _wanderStart <= Time::timestamp()) {
        _state = DroneState::SEARCHING;
        _wanderStart = 0ll;
        _acceleration = 0;
        _speed = glm::vec2(0);
    }
}

void Drone::_pursueAsteroid() 
{
    if (_pursueStart == 0ll) {
        _pursueStart = Time::timestamp();
    }

    auto distance = glm::distance(_targetAsteroid->getPosition(), _sprite->getPosition());
    if (distance < DRONE_SIGHT_RADIUS) {
        auto diff = _targetAsteroid->getPosition() - _sprite->getPosition();
        _angle = glm::atan(diff.y, diff.x);
        _acceleration = glm::sqrt(diff.x * diff.x + diff.y * diff.y) * 0.00002;
    } else {
        _state = DroneState::WANDERING;
        _pursueStart = 0ll;
        _acceleration = 0;
        _speed = glm::vec2(0);
    }

    if (Time::milliseconds(_pursueDuration) + _pursueStart <= Time::timestamp()) {
        _state = DroneState::WANDERING;
        _pursueStart = 0ll;
        _acceleration = 0;
        _speed = glm::vec2(0);
    }
}

void Drone::_beamAsteroid() {}

void Drone::_mineAsteroid() {}
