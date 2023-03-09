#include "droneFactory.h"

DroneFactory::DroneFactory(SpriteRenderer& spriteRenderer) : _spriteRenderer(spriteRenderer)
{
}

Drone* DroneFactory::createDrone() {
    auto sprite = _spriteRenderer.createSprite("playerShip1_blue.png");
    auto drone = new Drone(sprite);
    return drone;
}