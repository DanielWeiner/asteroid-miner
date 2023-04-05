#ifndef SPRITE_PHYSICS_ENGINE_H_
#define SPRITE_PHYSICS_ENGINE_H_

#include "time.h"

#include <box2d/b2_world.h>
#include <unordered_set>

class Sprite;

class SpritePhysicsEngine {
public:
    SpritePhysicsEngine(b2World& world, float timeStep, int velocityIterations, int positionIterations);
    void addSprite(Sprite* sprite);
    void removeSprite(Sprite* sprite);
    bool update();
private:
    b2World&                    _world;
    std::unordered_set<Sprite*> _sprites;
    long long                   _lastTime = Time::timestamp();
    float                       _timeStep;
    int                         _velocityIterations;
    int                         _positionIterations;
};

#endif