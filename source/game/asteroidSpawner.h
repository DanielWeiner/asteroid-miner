#ifndef ASTEROID_SPAWNER_H_
#define ASTEROID_SPAWNER_H_

#include "../spriteFactory.h"
#include "../event.h"
#include "asteroid.h"
#include "../flatScene.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class AsteroidSpawner {
public:
    AsteroidSpawner(std::shared_ptr<SpriteFactory> factory, std::shared_ptr<FlatScene> scene);
    void step();
    std::vector<std::unique_ptr<Asteroid>>& asteroids();
private:
    static constexpr glm::vec2             CHUNK_SIZE          = glm::vec2(500.f);
    static constexpr int                   ASTEROIDS_PER_CHUNK = 5;

    std::shared_ptr<SpriteFactory>         _spriteFactory;
    std::shared_ptr<FlatScene>             _scene;

    glm::vec2                              _topLeft            = glm::vec2(0);
    glm::vec2                              _bottomRight        = glm::vec2(0);

    std::vector<std::unique_ptr<Asteroid>> _asteroids;

    void      _populateChunks();
    void      _fillWithAsteroids(glm::vec2 topLeft, glm::vec2 bottomRight);
};

#endif