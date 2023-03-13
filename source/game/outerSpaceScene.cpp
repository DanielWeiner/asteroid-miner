#include "outerSpaceScene.h"

OuterSpaceScene::OuterSpaceScene(
    DroneFactory& droneFactory,
    SpriteRenderer& spriteRenderer,
    FlatScene&     scene
)
: _droneFactory(droneFactory), 
  _asteroidSpawner(spriteRenderer, scene), 
  _scene(scene) {}

Util::PointerSpan<std::vector<std::unique_ptr<Drone>>> OuterSpaceScene::drones()
{    
    return _drones;
}

Util::PointerSpan<std::vector<std::unique_ptr<Asteroid>>> OuterSpaceScene::asteroids()
{
    return _asteroidSpawner.asteroids();
}

void OuterSpaceScene::handleEvent(const Event& event)
{
}

void OuterSpaceScene::step() 
{
    _asteroidSpawner.step();
    for (Drone& drone : drones()) {
        drone.step(*this);
    }
}

void OuterSpaceScene::init() 
{
    for (int i = 0; i < 100; i++) {
        Drone* drone = _droneFactory.createDrone();
        drone->moveTo(_scene.toWorldCoordinates(glm::vec2(0)) + _scene.getWorldSize() / 2.f);
        _drones.push_back(std::unique_ptr<Drone>(drone));
    }
}

void OuterSpaceScene::render(LineRenderer& lineRenderer)
{
    for (Drone& drone: drones()) {
        drone.render(lineRenderer);
    }
}
