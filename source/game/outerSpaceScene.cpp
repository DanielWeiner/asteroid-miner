#include "outerSpaceScene.h"

OuterSpaceScene::OuterSpaceScene(
    DroneFactory& droneFactory,
    FlatScene&     scene)
: _droneFactory(droneFactory), _scene(scene) {}

std::span<std::unique_ptr<Drone>> OuterSpaceScene::drones()
{
    std::span<std::unique_ptr<Drone>> span{ _drones };
    
    return span;
}

void OuterSpaceScene::handleEvent(const Event& event)
{
}

void OuterSpaceScene::step() 
{
    for (auto& drone : drones()) {
        drone->step(*this);
    }
}

void OuterSpaceScene::init() 
{
    for (int i = 0; i < 10000; i++) {
        std::unique_ptr<Drone> drone(_droneFactory.createDrone());
        drone->moveTo(_scene.toWorldCoordinates(glm::vec2(0)) + _scene.getWorldSize() / 2.f);
        _drones.push_back(std::move(drone));
    }
}

void OuterSpaceScene::render(LineRenderer& lineRenderer)
{
    for (auto& drone: drones()) {
        drone->render(lineRenderer);
    }
}
