#include "drone.h"
#include "../time.h"
#include "../random.h"
#include "outerSpaceScene.h"
#include "../lineRenderer.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/gtx/norm.hpp>
#include "constants.h"
#include <iostream>
#include <format>

Drone::Drone(std::unique_ptr<Sprite> &&sprite) : _sprite(std::move(sprite)) {}

void Drone::setSpeed(glm::vec2 speed) { _speed = speed; }

void Drone::moveTo(glm::vec2 position) 
{
    _sprite->moveTo(position - _sprite->getSize() / 2.f);
}

void Drone::step(OuterSpaceScene& scene) 
{
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
    case DroneState::INITIAL:
        _state = DroneState::WANDERING;
        break;
    }

    _speed += _acceleration;
    auto speedScalar = glm::length(_speed);
    
    if (speedScalar > _maxSpeed) {
        auto angle = glm::atan(_speed.y, _speed.x);
        _speed = glm::vec2(glm::cos(angle), glm::sin(angle)) * (float)_maxSpeed;
    }

    _sprite->move(_speed);

    _rotateDirection();
}

void Drone::render(LineRenderer& lineRenderer)
{
    if (_targetAsteroid) {
        //lineRenderer.lineTo(_sprite->getCenter(), _targetAsteroid->getCenter(), glm::vec4(0,1,0,1), 3.f);
    }
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

void Drone::calculateEdgeTangents()
{
    _sprite->calculateEdgeTangents();
}

void Drone::_accelerateToward(glm::vec2 point)
{
    auto difference = _sprite->getCenter() - glm::vec2(point);
    auto angle = glm::atan(difference.y, difference.x) + PI;

    _acceleration = glm::dvec2(glm::cos(angle), glm::sin(angle));
}

void Drone::_rotateDirection() {
    _sprite->rotateTo(HALF_PI + glm::atan(_speed.y, _speed.x));
}

void Drone::_searchForAsteroid(OuterSpaceScene& scene) 
{
    std::shared_ptr<Asteroid> closestAsteroid;
    float shortestDistance = MAX_FLOAT;
    for (auto asteroid : scene.asteroids()) {
        auto distance = glm::distance2(asteroid->getCenter(), _sprite->getCenter());
        if (distance < _sightRadius2) {
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
        auto direction = Random::uniform() * TWO_PI;
        _acceleration = glm::vec2(glm::cos(direction), glm::sin(direction));
        return;
    }

    if (Time::milliseconds(_wanderDuration) + _wanderStart <= Time::timestamp()) {
        _state = DroneState::SEARCHING;
        _wanderStart = 0ll;
        return;
    }
}

void Drone::_pursueAsteroid() 
{
    if (_pursueStart == 0ll) {
        _pursueStart = Time::timestamp();
        _accelerateToward(_targetAsteroid->getCenter());
        return;
    }

    if (glm::distance2(_sprite->getCenter(), _targetAsteroid->getCenter()) > _sightRadius2 || Time::milliseconds(_pursueDuration) + _pursueStart <= Time::timestamp()) {
        _targetAsteroid = nullptr;
        _state = DroneState::WANDERING;
        _pursueStart = 0ll;
        return;
    }

    _accelerateToward(_targetAsteroid->getCenter());
}

void Drone::_beamAsteroid() {}

void Drone::_mineAsteroid() {}
