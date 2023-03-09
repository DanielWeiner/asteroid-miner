#include "asteroidSpawner.h"

#include "../random.h"

#include <glm/ext.hpp>
#include <iostream>

AsteroidSpawner::AsteroidSpawner(SpriteRenderer& renderer, FlatScene& scene) 
: _spriteRenderer(renderer), _scene(scene)
{}

void AsteroidSpawner::setDensity(float density) 
{
    _asteroidsPerChunk = _chunkSize.x * _chunkSize.y * density;
}

void AsteroidSpawner::step() {
  _populateChunks();
  for (Asteroid &asteroid : _asteroids) {
    asteroid.step(_topLeft, _bottomRight);
  }
}

std::span<std::reference_wrapper<Asteroid>> AsteroidSpawner::asteroids()
{
    std::span<std::reference_wrapper<Asteroid>> asteroids{ _asteroids };

    return asteroids;
}

void AsteroidSpawner::_populateChunks() 
{   
    using vec2 = glm::vec2;
    using vec4 = glm::vec4;

    auto worldTopLeft = _scene.getWorldPosition();
    auto worldBottomRight = _scene.getWorldPosition() + _scene.getWorldSize();

    auto chunkTopLeft = glm::floor(worldTopLeft / _chunkSize);
    auto chunkBottomRight = glm::ceil(worldBottomRight / _chunkSize);

    auto corners = vec4(chunkTopLeft, chunkBottomRight);
    auto paddedCorners = corners + vec4(vec2(-_chunkPadding), vec2(_chunkPadding));

    auto worldCorners = paddedCorners * vec4(_chunkSize, _chunkSize);

    auto topLeft = vec2(glm::min(worldCorners.x, _topLeft.x), glm::min(worldCorners.y, _topLeft.y));
    auto bottomRight = vec2(glm::max(worldCorners.z, _bottomRight.x), glm::max(worldCorners.w, _bottomRight.y));

    if (topLeft == _topLeft && bottomRight == _bottomRight) {
        return;
    }

    _fillWithAsteroids(topLeft,                          vec2(bottomRight.x, _topLeft.y));
    _fillWithAsteroids(vec2(topLeft.x, _topLeft.y),      vec2(_topLeft.x, _bottomRight.y));
    _fillWithAsteroids(vec2(_bottomRight.x, _topLeft.y), vec2(bottomRight.x, _bottomRight.y));
    _fillWithAsteroids(vec2(topLeft.x, _bottomRight.y),  vec2(bottomRight.x, bottomRight.y));

    _topLeft = topLeft;
    _bottomRight = bottomRight;
}

void AsteroidSpawner::_fillWithAsteroids(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    auto chunks = (bottomRight - topLeft) / _chunkSize;
    auto numChunks = chunks.x * chunks.y;

    for (int i = 0; i < numChunks * _asteroidsPerChunk; i++) {
        auto sprite = _spriteRenderer.createSprite("Meteors/meteorGrey_med1.png");
        sprite->moveTo(glm::vec2(
            topLeft.x + Random::uniform() * (bottomRight.x - topLeft.x),
            topLeft.y + Random::uniform() * (bottomRight.y - topLeft.y)
        ));
        
        auto angle = Random::uniform() * TWO_PI;
        auto velocity = Random::normal(_avgSpeed, _speedSigma);

        auto speed = glm::vec2(glm::cos(angle) * velocity, glm::sin(angle) * velocity);
        auto spin = (Random::uniform()  * 2.f - 1.f) * _maxSpin;
  
        _asteroids.push_back(*new Asteroid(*sprite, speed, spin));
    }
}