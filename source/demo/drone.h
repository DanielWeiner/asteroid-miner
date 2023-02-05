#ifndef DEMO_DRONE_H_
#define DEMO_DRONE_H_

#include "../sprite.h"
#include <glm/glm.hpp>
#include <memory>

class DemoDrone {
public:
    DemoDrone(std::unique_ptr<Sprite>& sprite);
    void setSpeed(glm::vec2 speed);
    void moveTo(glm::vec2 position);
    void step();
    glm::vec2 getSpeed();
    glm::vec2 getPosition();
    glm::vec2 getNextPosition();
    glm::vec2 getSize();
    void update();
private:
    std::unique_ptr<Sprite> _sprite;
    glm::vec2               _speed;

    void _rotateDirection();
};

#endif