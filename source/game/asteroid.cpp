#include "asteroid.h"

#include <utility>

Asteroid::Asteroid(std::unique_ptr<Sprite> &&sprite, glm::vec2 speed, float spin) 
: 
_sprite(std::move(sprite)), 
_speed(speed),
_spin(spin) {}

void Asteroid::setSpeed(glm::vec2 speed) 
{
    _speed = speed;
}

glm::vec2 Asteroid::getSpeed()
{
    return _speed;
}

glm::vec2 Asteroid::getPosition()
{
    return _sprite->getPosition();
}

glm::vec2 Asteroid::getSize()
{
    return _sprite->getSize();
}

void Asteroid::step(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    _speed *= 1.f - _friction;

    auto size = glm::vec2(bottomRight - topLeft);
    auto position = getPosition() + _speed;
    auto relativePosition = position - topLeft;

    _sprite->moveTo(topLeft + relativePosition - size * glm::floor(relativePosition / size));
    _sprite->rotate(_spin);
}