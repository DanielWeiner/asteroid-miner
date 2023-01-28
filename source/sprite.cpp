#include "sprite.h"
#include "spriteBuffer.h"
#include "spriteSheet.h"
#include <glm/ext.hpp>

Sprite::Sprite(
    std::string spriteName, 
    unsigned int id, 
    std::shared_ptr<SpriteSheet>    spriteSheet, 
    std::shared_ptr<SpriteBuffer>   spriteBuffer
) : 
    _name(spriteName), 
    _id(id), 
    _sheet(spriteSheet),
    _buffer(spriteBuffer)
{}

Sprite& Sprite::operator=(const Sprite& sprite)
{
    _name = sprite._name;
    _x = sprite._x;
    _y = sprite._y;
    _width = sprite._width;
    _height = sprite._height;
    _buffer = sprite._buffer;
    _sheet = sprite._sheet;

    updateModelMatrix();
    updateTextureMatrix();

    return *this;
}

void Sprite::moveTo(float x, float y) 
{
    return move(x - _x, y - _y);
}

void Sprite::moveTo(glm::vec2 xy) 
{
    return moveTo(xy.x, xy.y);
}

void Sprite::move(float x, float y) 
{
    _x += x;
    _y += y;

    updateModelMatrix();
}

void Sprite::move(glm::vec2 xy) 
{
    return move(xy.x, xy.y);
}

void Sprite::rotate(float radians) 
{
    _rotate += radians;

    updateModelMatrix();
}

void Sprite::rotateTo(float radians) 
{
    return rotate(radians - _rotate);
}

void Sprite::scaleBy(float x, float y) 
{
    _width *= x;
    _height *= y;

    updateModelMatrix();
}

void Sprite::scaleBy(glm::vec2 xy) 
{
    return scaleBy(xy.x, xy.y);
}

std::string Sprite::getName()
{
    return _name;
}

glm::vec2 Sprite::getPosition()
{
    return glm::vec2(_x, _y);
}

glm::vec2 Sprite::getRawDimensions()
{
    return glm::vec2(_width, _height);
}

void Sprite::updateModelMatrix()
{
    _buffer->initializeModel(_id, glm::mat4(1.0f));
    _buffer->translateModel(_id, glm::vec3(_x, _y, 0.0f));

    _buffer->translateModel(_id, glm::vec3(0.5f * _width, 0.5f * _height, 0.0f));
    _buffer->rotateModel(_id, _rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    _buffer->translateModel(_id, glm::vec3(-0.5f * _width, -0.5f * _height, 0.0f));

    _buffer->scaleModel(_id, glm::vec3(_width, _height, 1.0f));
}

void Sprite::updateTextureMatrix() 
{
    _buffer->setTexture(_id, _sheet->getSpriteMatrix(_name.c_str()));   
}

bool Sprite::pointIsInHitbox(float x, float y)
{
    auto modelMatrix = _buffer->getModelMatrix(_id);
    glm::vec2 pos = modelMatrix / glm::vec4(x, y, 0.0, 1.0);

    if (!(pos.x < 1.f && pos.x >= 0.f && pos.y < 1.f && pos.y >= 0.f)) {
        return false;
    }

    return _sheet->pixelAt(_name.c_str(), pos.x, pos.y).a;
}

Sprite::~Sprite() 
{
    if (_buffer) {
        _buffer->destroyResource(_id);
    }
}
