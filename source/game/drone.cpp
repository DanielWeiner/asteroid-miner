#include "drone.h"
#include <cmath>
#include <numbers>


Drone::Drone(std::unique_ptr<Sprite>& sprite) : _sprite(std::move(sprite)) {}

void Drone::setSpeed(glm::vec2 speed) 
{
    _speed = speed;
}

void Drone::moveTo(glm::vec2 position) 
{
    _sprite->moveTo(position);
}

void Drone::step() 
{
    _sprite->move(_speed);
    _sprite->rotateTo(std::numbers::pi - atan2(_speed.x, _speed.y));
}

glm::vec2 Drone::getSpeed()
{
    return _speed;
}

Sprite& Drone::getSprite()
{
    return *_sprite;
}

glm::vec2 Drone::getPosition()
{
    return _sprite->getPosition();
}

glm::vec2 Drone::getSize()
{
    return _sprite->getSize();
}

void Drone::_rotateDirection() 
{
    _sprite->rotateTo(std::numbers::pi - atan2(_speed.x, _speed.y));
}
