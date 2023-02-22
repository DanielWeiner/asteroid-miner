#ifndef OUTER_SPACE_SCENE_H_
#define OUTER_SPACE_SCENE_H_


#include "../flatScene.h"
#include "../spriteFactory.h"
#include "droneFactory.h"
#include "asteroidSpawner.h"
#include "drone.h"

class LineRenderer;

#include <vector>
#include <memory>
#include <span>

class OuterSpaceScene {
public:
    OuterSpaceScene(
        std::unique_ptr<DroneFactory>    &&droneFactory, 
        std::unique_ptr<AsteroidSpawner> &&asteroidSpawner,
        std::shared_ptr<FlatScene>       scene
    );

    std::span<std::shared_ptr<Asteroid>> asteroids();
    std::span<std::unique_ptr<Drone>>    drones();

    void handleEvent(const Event& event);

    void step();
    void init();
    void render(LineRenderer& lineRenderer);
private:
    std::unique_ptr<DroneFactory>       _droneFactory;
    std::unique_ptr<AsteroidSpawner>    _asteroidSpawner;
    std::vector<std::unique_ptr<Drone>> _drones;
    std::shared_ptr<FlatScene>          _scene;
};

#endif