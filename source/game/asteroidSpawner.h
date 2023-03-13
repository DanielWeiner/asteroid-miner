#ifndef ASTEROID_SPAWNER_H_
#define ASTEROID_SPAWNER_H_

#include "constants.h"
#include "../util/pointerSpan.h"
#include "../spriteFactory.h"
#include "../event.h"
#include "asteroid.h"
#include "../flatScene.h"

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class AsteroidSpawner {
public:
    AsteroidSpawner(
        SpriteRenderer& renderer, 
        FlatScene&      scene
    );
    void setDensity(float density);
    void step();
    Util::PointerSpan<std::vector<std::unique_ptr<Asteroid>>> asteroids();
private:

    SpriteRenderer&                        _spriteRenderer;
    FlatScene&                             _scene;

    glm::vec2                              _topLeft           = glm::vec2(0);
    glm::vec2                              _bottomRight       = glm::vec2(0);

    int                                    _chunkPadding      = ASTEROID_CHUNK_PADDING;
    glm::vec2                              _chunkSize         = ASTEROID_CHUNK_SIZE;
    double                                 _density           = ASTEROID_DENSITY;
    float                                  _asteroidsPerChunk = _chunkSize.x * _chunkSize.y * _density;
    double                                 _avgSpeed          = ASTEROID_AVG_SPEED;
    double                                 _speedSigma        = ASTEROID_SPEED_SIGMA;
    double                                 _maxSpin           = ASTEROID_MAX_SPIN;

    std::vector<std::unique_ptr<Asteroid>> _asteroids;

    void      _populateChunks();
    void      _fillWithAsteroids(glm::vec2 topLeft, glm::vec2 bottomRight);
};

#endif