#ifndef DRONE_FACTORY_H_
#define DRONE_FACTORY_H_

#include "drone.h"
#include "../spriteRenderer.h"

#include <memory>

class DroneFactory {
public:
    DroneFactory(SpriteRenderer& spriteRenderer);
    Drone* createDrone();

private:
    SpriteRenderer& _spriteRenderer;
};

#endif