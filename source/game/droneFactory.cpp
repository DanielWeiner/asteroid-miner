#include "droneFactory.h"

DroneFactory::DroneFactory(std::unique_ptr<SpriteFactory>&& spriteFactory)
: _spriteFactory(std::move(spriteFactory)) {}

std::unique_ptr<Drone> DroneFactory::createDrone() {
    auto sprite = _spriteFactory->createSprite("playerShip1_blue.png");
    auto drone = std::make_unique<Drone>(std::move(sprite));
    return std::move(drone);
}