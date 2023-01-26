#include "sprite.h"
#include <glm/ext.hpp>

Sprite::Sprite(const char* spriteName, glm::mat4* buffer) : 
    _spriteName(spriteName), _buffer(buffer)
{}

Sprite& Sprite::moveTo(float x, float y) 
{
    return move(x - _x, y - _y);
}

Sprite& Sprite::moveTo(glm::vec2 xy) 
{
    return moveTo(xy.x, xy.y);
}

Sprite& Sprite::move(float x, float y) 
{
    _x += x;
    _y += y;

    updateModelMatrix();

    return *this;
}

Sprite& Sprite::move(glm::vec2 xy) 
{
    return move(xy.x, xy.y);
}

Sprite& Sprite::rotate(float radians) 
{
    _rotate += radians;

    updateModelMatrix();

    return *this;
}

Sprite& Sprite::rotateTo(float radians) 
{
    return rotate(radians - _rotate);
}

Sprite& Sprite::scaleBy(float x, float y) 
{
    _width *= x;
    _height *= y;

    updateModelMatrix();

    return *this;
}

Sprite& Sprite::scaleBy(glm::vec2 xy) 
{
    return scaleBy(xy.x, xy.y);
}

const char* Sprite::getName()
{
    return _spriteName;
}

glm::vec2 Sprite::getPosition()
{
    return glm::vec2(_x, _y);
}

void Sprite::updateModelMatrix()
{
    *_buffer = glm::mat4(1.0f);
    *_buffer = glm::translate(*_buffer, glm::vec3(_x, _y, 0.0f));  

    *_buffer = glm::translate(*_buffer, glm::vec3(0.5f * _width, 0.5f * _height, 0.0f)); 
    *_buffer = glm::rotate(*_buffer, _rotate, glm::vec3(0.0f, 0.0f, 1.0f)); 
    *_buffer = glm::translate(*_buffer, glm::vec3(-0.5f * _width, -0.5f * _height, 0.0f));

    *_buffer = glm::scale(*_buffer, glm::vec3(_width, _height, 1.0f));
}
