#ifndef ASTEROID_H_
#define ASTEROID_H_

#include "constants.h"
#include "../sprite.h"
#include <glm/glm.hpp>

class Asteroid {
public:
    Asteroid(Sprite* sprite, glm::vec2 speed, float spin);
    unsigned int id() const;
    void         setSpeed(glm::vec2 speed);
    glm::vec2    getSpeed();
    glm::vec2    getPosition();
    glm::vec2    getCenter();
    glm::vec2    getSize();
    void         step(glm::vec2 topLeft, glm::vec2 bottomRight);
private:
    std::unique_ptr<Sprite> _sprite;
    glm::vec2               _speed = glm::vec2(0.f);
    float                   _spin = 0.f;
    glm::vec2               _baseSpeed;
    float                   _speedDecay = ASTEROID_SPEED_DECAY;
};

#endif