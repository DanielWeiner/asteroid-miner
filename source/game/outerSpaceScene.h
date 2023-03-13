#ifndef OUTER_SPACE_SCENE_H_
#define OUTER_SPACE_SCENE_H_

#include "../util/pointerSpan.h"
#include "../flatScene.h"
#include "../spriteFactory.h"
#include "droneFactory.h"
#include "asteroidSpawner.h"
#include "drone.h"

class LineRenderer;

#include <vector>
#include <memory>

class OuterSpaceScene {
public:
    OuterSpaceScene(
        DroneFactory& droneFactory,
        SpriteRenderer& spriteRenderer,
        FlatScene&    scene
    );

    Util::PointerSpan<std::vector<std::unique_ptr<Drone>>>    drones();
    Util::PointerSpan<std::vector<std::unique_ptr<Asteroid>>> asteroids();

    void handleEvent(const Event& event);

    void step();
    void init();
    void render(LineRenderer& lineRenderer);
private:
    DroneFactory&                       _droneFactory;
    std::vector<std::unique_ptr<Drone>> _drones;
    AsteroidSpawner                     _asteroidSpawner;
    FlatScene&                          _scene;
};

#endif