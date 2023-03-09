#include "droneFactory.h"

DroneFactory::DroneFactory(SpriteRenderer& spriteRenderer) : _spriteRenderer(spriteRenderer)
{
}

Drone* DroneFactory::createDrone() {
    return new Drone(*_spriteRenderer.createSprite("playerShip1_blue.png"));
}