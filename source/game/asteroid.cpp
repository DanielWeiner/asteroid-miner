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
}