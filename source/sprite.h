#ifndef SPRITE_H_
#define SPRITE_H_

#include <glm/glm.hpp>

class Sprite {
public:
    Sprite(const char* spriteName);

    Sprite& moveTo(float x, float y);
    Sprite& moveTo(glm::vec2 xy);

    Sprite& move(float x, float y);
    Sprite& move(glm::vec2 xy);

    Sprite& rotate(float degrees);
    Sprite& rotateTo(float degrees);

    Sprite& scaleBy(float x, float y);
    Sprite& scaleBy(glm::vec2 xy);

    const char* getName();

    glm::mat4* getModelMatrix();
private:
    const char* _spriteName;

    float _x = 0;
    float _y = 0;
    float _width = 1;
    float _height = 1;
    float _rotate = 0;

    glm::mat4 _transform;
};

#endif