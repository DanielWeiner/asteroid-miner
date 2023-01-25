#include "sprite.h"
#include <glm/ext.hpp>

Sprite::Sprite(const char* spriteName) : 
    _spriteName(spriteName),
    _transform(glm::mat4(1.0))
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

    return *this;
}

Sprite& Sprite::move(glm::vec2 xy) 
{
    return move(xy.x, xy.y);
}

Sprite& Sprite::rotate(float degrees) 
{
    _rotate += degrees;

    return *this;
}

Sprite& Sprite::rotateTo(float degrees) 
{
    float actualDegrees = (int)degrees % 360;

    return rotate(actualDegrees - _rotate);
}

Sprite& Sprite::scaleBy(float x, float y) 
{
    _width *= x;
    _height *= y;

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

glm::mat4* Sprite::getModelMatrix()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(_x, _y, 0.0f));  

    model = glm::translate(model, glm::vec3(0.5f * _width, 0.5f * _height, 0.0f)); 
    model = glm::rotate(model, glm::radians(_rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
    model = glm::translate(model, glm::vec3(-0.5f * _width, -0.5f * _height, 0.0f));

    model = glm::scale(model, glm::vec3(_width, _height, 1.0f));
    _transform = model;

    return &_transform;
}
