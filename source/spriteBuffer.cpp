#include "spriteBuffer.h"

void SpriteBuffer::rotateModel(unsigned int id, float radians, glm::vec3 point) 
{
    _models[_resourceIds[id]] = glm::rotate(_models[_resourceIds[id]], radians, point);
}

void SpriteBuffer::scaleModel(unsigned int id, glm::vec3 point) 
{
    _models[_resourceIds[id]] = glm::scale(_models[_resourceIds[id]], point);
}

void SpriteBuffer::translateModel(unsigned int id, glm::vec3 point) 
{
    _models[_resourceIds[id]] = glm::translate(_models[_resourceIds[id]], point);
}

void SpriteBuffer::setTexture(unsigned int id, glm::mat4 matrix) {
    _texturesDirty = true;
    _textures[_resourceIds[id]] = matrix;
}

void SpriteBuffer::initializeModel(unsigned int id, glm::mat4 &&matrix) {
    _models[_resourceIds[id]] = std::move(matrix);
}

bool SpriteBuffer::areTexturesDirty()
{
    return _texturesDirty;
}

void SpriteBuffer::resetDirtyFlag() 
{
    _texturesDirty = false;
}

unsigned int SpriteBuffer::size()
{
    return _models.size();
}

glm::mat4* SpriteBuffer::modelData()
{

    return &_models[0];
}

glm::mat4* SpriteBuffer::textureData()
{
    return &_textures[0];
}

glm::mat4 SpriteBuffer::getModelMatrix(unsigned int id)
{
    return glm::mat4(_models[_resourceIds[id]]);
}

unsigned int SpriteBuffer::createResource()
{
    unsigned int lastResourceId = _lastResourceId++;
    auto index = _models.size();

    _models.push_back(glm::mat4(1.0));
    _textures.push_back(glm::mat4(1.0));

    _resourceIds[lastResourceId] = index;
    return lastResourceId;
}

void SpriteBuffer::destroyResource(unsigned int id) 
{   
    auto resourceIndex = _resourceIds[id];

    _resourceIds.erase(id);
    for (auto [ resourceId, index ] : _resourceIds) {
        if (index > resourceIndex) {
            --_resourceIds[resourceId];
        }
    }
    
    _models.erase(_models.begin() + resourceIndex);
    _textures.erase(_textures.begin() + resourceIndex);

    _texturesDirty = true;
}
