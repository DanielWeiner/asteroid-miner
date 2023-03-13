#include "asteroid.h"

Asteroid::Asteroid(Sprite* sprite, glm::vec2 speed, float spin) 
:
_sprite(sprite), 
_speed(speed),
_spin(spin),
_baseSpeed(speed) {}

unsigned int Asteroid::id() const
{
    return _sprite->id();
}

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

glm::vec2 Asteroid::getCenter()
{
    return _sprite->getCenter();
}

glm::vec2 Asteroid::getSize()
{
    return _sprite->getSize();
}

void Asteroid::step(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    if (_baseSpeed != _speed) {
        _speed += glm::sign(_baseSpeed - _speed) * _speedDecay * glm::sqrt(glm::abs(_baseSpeed - _speed));
    }
    auto size = glm::vec2(bottomRight - topLeft);
    auto position = _sprite->getNextPosition() + _speed;
    auto relativePosition = position - topLeft;

    _sprite->moveTo(topLeft + relativePosition - size * glm::floor(relativePosition / size));
    _sprite->rotate(_spin);
}