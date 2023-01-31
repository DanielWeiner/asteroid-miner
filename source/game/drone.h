#ifndef DRONE_H_
#define DRONE_H_

#include "../sprite.h"
#include <glm/glm.hpp>
#include <memory>

class Drone {
public:
    Drone(std::unique_ptr<Sprite>& sprite);
    void setSpeed(glm::vec2 speed);
    void moveTo(glm::vec2 position);
    void step();
    glm::vec2 getSpeed();
    Sprite& getSprite();
    glm::vec2 getPosition();
    glm::vec2 getSize();
private:
    std::unique_ptr<Sprite> _sprite;
    glm::vec2               _speed;

    void _rotateDirection();
};

#endif