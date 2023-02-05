#ifndef DRONE_FACTORY_H_
#define DRONE_FACTORY_H_

#include "drone.h"
#include "../spriteFactory.h"

#include <memory>

class DroneFactory {
public:
    DroneFactory(std::unique_ptr<SpriteFactory>&& spriteFactory);
    std::unique_ptr<Drone> createDrone();

private:
    std::unique_ptr<SpriteFactory> _spriteFactory;
};

#endif