#include "spritePhysicsEngine.h"
#include "sprite.h"

SpritePhysicsEngine::SpritePhysicsEngine(b2World& world, float timeStep, int velocityIterations, int positionIterations)
: _world(world),
  _timeStep(timeStep),
  _velocityIterations(velocityIterations),
  _positionIterations(positionIterations)
{}

void SpritePhysicsEngine::addSprite(Sprite* sprite)
{
    _sprites.insert(sprite);
}

void SpritePhysicsEngine::removeSprite(Sprite* sprite)
{
    _sprites.erase(sprite);
}

bool SpritePhysicsEngine::update() 
{
    long long currentTime = Time::timestamp();
    long long deltaTime = currentTime - _lastTime;

    int accumulatedTime = 0;
    while (accumulatedTime < deltaTime) {
        long long stepStartTime = Time::timestamp();
        _world.Step(_timeStep, _velocityIterations, _positionIterations);
        long long stepDuration = (Time::timestamp() - stepStartTime);
        long long timeStepDuration = _timeStep * 1000 - stepDuration; // timeStepDuration is the time left after the physics step  

        _lastTime += timeStepDuration;
        accumulatedTime += timeStepDuration;
    }

    if (accumulatedTime > 0) {
        for (auto sprite : _sprites) {
            sprite->update();
        }
        return true;
    }

    return false;
}
