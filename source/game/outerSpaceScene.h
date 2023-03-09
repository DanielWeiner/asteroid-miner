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
        DroneFactory& droneFactory,
        FlatScene&     scene
    );

    std::span<std::unique_ptr<Drone>> drones();

    void handleEvent(const Event& event);

    void step();
    void init();
    void render(LineRenderer& lineRenderer);
private:
    DroneFactory&                       _droneFactory;
    std::vector<std::unique_ptr<Drone>> _drones;
    FlatScene&                          _scene;
};

#endif