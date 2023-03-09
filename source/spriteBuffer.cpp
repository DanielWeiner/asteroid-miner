#include "spriteBuffer.h"

namespace {
    constexpr std::size_t MAT4_SIZE = sizeof(glm::mat4) / sizeof(float);
    constexpr std::size_t DATA_SIZE = MAT4_SIZE + 1;
}

void SpriteBuffer::setTexture(unsigned int id, std::size_t spriteId) {
    _setTexture(id, spriteId);
}

void SpriteBuffer::setModel(unsigned int id, glm::mat4 matrix) {
    _setModel(id, matrix);
}

unsigned int SpriteBuffer::size()
{
    return _models.size();
}
float* SpriteBuffer::modelData()
{
    return _models.data();
}

glm::mat4* SpriteBuffer::getModelMatrix(unsigned int id)
{
    return _getModel(id);
}

unsigned int SpriteBuffer::createResource()
{
    unsigned int lastResourceId = _lastResourceId++;
    auto index = _models.size();

    glm::mat4 model(1.0);
    _models.insert(_models.end(), glm::value_ptr(model), glm::value_ptr(model) + MAT4_SIZE);
    _models.push_back(-1.f);

    _resourceIds[lastResourceId] = index;
    return lastResourceId;
}

void SpriteBuffer::destroyResource(unsigned int id) 
{   
    auto resourceIndex = _resourceIds[id];

    _resourceIds.erase(id);
    for (auto [ resourceId, index ] : _resourceIds) {
        if (index > resourceIndex) {
            _resourceIds[resourceId] -= DATA_SIZE;
        }
    }
    
    _models.erase(_models.begin() + resourceIndex, _models.begin() + resourceIndex + DATA_SIZE);
}

void SpriteBuffer::moveToEnd(unsigned int id) 
{
    float* model = (float*)_getModel(id);
    float texture = _getTexture(id);
    destroyResource(id);
    _resourceIds[id] = _models.size();
    _models.insert(_models.end(), model, model + MAT4_SIZE);
    _models.push_back(texture);
}

unsigned int SpriteBuffer::getStep()
{
    return _step;
}

unsigned int SpriteBuffer::getNextStep()
{
    return (_step + 1) % 2; 
}

void SpriteBuffer::step() 
{
    _step = getNextStep();
}

std::size_t SpriteBuffer::dataSize()
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
