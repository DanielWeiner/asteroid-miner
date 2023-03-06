#include "outerSpaceScene.h"

OuterSpaceScene::OuterSpaceScene(
    std::unique_ptr<DroneFactory>    &&droneFactory,
    std::unique_ptr<AsteroidSpawner> &&asteroidSpawner,
    std::shared_ptr<FlatScene>         scene)
: _droneFactory(std::move(droneFactory)), _asteroidSpawner(std::move(asteroidSpawner)), _scene(scene) {}

std::span<std::shared_ptr<Asteroid>> OuterSpaceScene::asteroids() {
  std::span<std::shared_ptr<Asteroid>> span{_asteroidSpawner->asteroids()};

  return span;
}

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

    _asteroidSpawner->step();
}

void OuterSpaceScene::init() 
{
    for (int i = 0; i < 1; i++) {
        auto drone = _droneFactory->createDrone();
        drone->calculateEdgeTangents();
        drone->moveTo(_scene->toWorldCoordinates(glm::vec2(0)) + _scene->getWorldSize() / 2.f);
        _drones.push_back(std::move(drone));
    }
}

void OuterSpaceScene::render(LineRenderer& lineRenderer)
{
    for (auto& drone: drones()) {
        drone->render(lineRenderer);
    }
}
