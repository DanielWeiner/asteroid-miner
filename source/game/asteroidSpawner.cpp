#include "asteroidSpawner.h"

#include "../random.h"

#include <glm/ext.hpp>
#include <iostream>

AsteroidSpawner::AsteroidSpawner(std::shared_ptr<SpriteFactory> factory,
                                 std::shared_ptr<FlatScene> scene) 
: _spriteFactory(factory), _scene(scene)
{}

void AsteroidSpawner::step() {
  _populateChunks();
  for (auto &asteroid : _asteroids) {
    asteroid->step(_topLeft, _bottomRight);
  }
}

std::vector<std::unique_ptr<Asteroid>>& AsteroidSpawner::asteroids()
{
    return _asteroids;
}

void AsteroidSpawner::_populateChunks() 
{   
    auto screenTopLeft = (glm::floor(_scene->getWorldPosition() / CHUNK_SIZE) - 1.f) * CHUNK_SIZE;
    auto screenBottomRight = (glm::ceil((_scene->getWorldPosition() + _scene->getWorldSize()) / CHUNK_SIZE) + 2.f) * CHUNK_SIZE;

    auto topLeft = glm::vec2(glm::min(screenTopLeft.x, _topLeft.x), glm::min(screenTopLeft.y, _topLeft.y));
    auto bottomRight = glm::vec2(glm::max(screenBottomRight.x, _bottomRight.x), glm::max(screenBottomRight.y, _bottomRight.y));

    if (topLeft == _topLeft && bottomRight == _bottomRight) {
        return;
    }

    _fillWithAsteroids(topLeft,                               glm::vec2(bottomRight.x, _topLeft.y));
    _fillWithAsteroids(glm::vec2(topLeft.x, _topLeft.y),      glm::vec2(_topLeft.x, _bottomRight.y));
    _fillWithAsteroids(glm::vec2(_bottomRight.x, _topLeft.y), glm::vec2(bottomRight.x, _bottomRight.y));
    _fillWithAsteroids(glm::vec2(topLeft.x, _bottomRight.y),  glm::vec2(bottomRight.x, bottomRight.y));

    _topLeft = topLeft;
    _bottomRight = bottomRight;
}

void AsteroidSpawner::_fillWithAsteroids(glm::vec2 topLeft, glm::vec2 bottomRight) 
{
    auto chunks = (bottomRight - topLeft) / CHUNK_SIZE;
    auto numChunks = chunks.x * chunks.y;

    for (int i = 0; i < numChunks * ASTEROIDS_PER_CHUNK; i++) {
        auto sprite = _spriteFactory->createSprite("Meteors/meteorGrey_med1.png");
        sprite->moveTo(glm::vec2(
            topLeft.x + Random::uniform() * (bottomRight.x - topLeft.x),
            topLeft.y + Random::uniform() * (bottomRight.y - topLeft.y)
        ));
        
        auto angle = Random::uniform() * 2.0 * glm::pi<double>();
        auto velocity = Random::normal(2, 4);

        auto speed = glm::vec2(glm::cos(angle) * velocity, glm::sin(angle) * velocity);
        auto spin = Random::uniform() * 0.03;
        
        auto asteroid = std::make_unique<Asteroid>(std::move(sprite), speed, spin);
        _asteroids.push_back(std::move(asteroid));
    }
}