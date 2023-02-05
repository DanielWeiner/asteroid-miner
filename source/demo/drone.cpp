#include "drone.h"
#include "../constants.h"


DemoDrone::DemoDrone(std::unique_ptr<Sprite>& sprite) : _sprite(std::move(sprite)) {}

void DemoDrone::setSpeed(glm::vec2 speed) 
{
    _speed = speed;
}

void DemoDrone::moveTo(glm::vec2 position) 
{
    _sprite->moveTo(position);
}

void DemoDrone::step() 
{
    _sprite->move(_speed);
    _sprite->rotateTo(HALF_PI + glm::atan(_speed.y, _speed.x));
    _sprite->update();
}

glm::vec2 DemoDrone::getSpeed()
{
    return _speed;
}

glm::vec2 DemoDrone::getPosition()
{
    return _sprite->getPosition();
}

glm::vec2 DemoDrone::getNextPosition()
{
    return _sprite->getNextPosition();
}

glm::vec2 DemoDrone::getSize()
{
    return _sprite->getSize();
}

void DemoDrone::_rotateDirection() 
{
    _sprite->rotateTo(HALF_PI + glm::atan(_speed.y, _speed.x));
}

void DemoDrone::update() {
    _sprite->update();
}