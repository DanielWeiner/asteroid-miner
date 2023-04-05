#include "spriteBuffer.h"
#include "spriteResource.h"
#include <box2d/b2_body.h>

#include <iostream>

namespace {
    constexpr int MAT4_SIZE = sizeof(glm::mat4) / sizeof(float);
    constexpr int DATA_SIZE = MAT4_SIZE + 3;
}

void SpriteBuffer::setTexture(unsigned int id, std::size_t spriteId) {
    _setTexture(id, spriteId);
}

void SpriteBuffer::setModel(unsigned int id, glm::mat4 matrix) {
    _setModel(id, matrix);
}

void SpriteBuffer::setOpacity(unsigned int id, float opacity) 
{
    _setOpacity(id, opacity);
}

const unsigned int SpriteBuffer::size() const
{
    return _models.size();
}
const float* SpriteBuffer::modelData() const
{
    return _models.data();
}

glm::mat4* SpriteBuffer::getModelMatrix(unsigned int id)
{
    return _getModel(id);
}

SpriteResource SpriteBuffer::createResource(b2Body& body, unsigned int textureSpriteIndex, bool useLinearScaling)
{
    unsigned int lastResourceId = _lastResourceId++;
    auto index = _models.size();

    glm::mat4 model(1.0);
    _models.insert(_models.end(), glm::value_ptr(model), glm::value_ptr(model) + MAT4_SIZE);
    _models.push_back(textureSpriteIndex); // sprite index
    _models.push_back(1.f);  // opacity
    _models.push_back(useLinearScaling); // use linear scaling

    _resourceIds[lastResourceId] = index;
    return SpriteResource{
        lastResourceId, 
        body
    };
}

void SpriteBuffer::destroyResource(SpriteResource& resource) 
{
    auto resourceIndex = _resourceIds[resource.id];

    _resourceIds.erase(resource.id);
    for (auto [ resourceId, index ] : _resourceIds) {
        if (index > resourceIndex) {
            _resourceIds[resourceId] -= DATA_SIZE;
        }
    }
    
    _models.erase(_models.begin() + resourceIndex, _models.begin() + resourceIndex + DATA_SIZE);
}

const int SpriteBuffer::dataSize()
{
    return DATA_SIZE;
}

glm::mat4* SpriteBuffer::_getModel(unsigned int id)
{
    return (glm::mat4*)&_models[_resourceIds[id]];
}

float SpriteBuffer::_getTexture(unsigned int id)
{
    return _models[_resourceIds[id] + MAT4_SIZE];
}

void SpriteBuffer::_setModel(unsigned int id, glm::mat4 model) 
{
    *((glm::mat4*)&_models[_resourceIds[id]]) = model;
}

void SpriteBuffer::_setTexture(unsigned int id, float textureId) 
{
    _models[_resourceIds[id] + MAT4_SIZE] = textureId;
}

void SpriteBuffer::_setOpacity(unsigned int id, float opacity) 
{
    _models[_resourceIds[id] + MAT4_SIZE + 1] = opacity;
}