#ifndef SPRITE_BUFFER_H_
#define SPRITE_BUFFER_H_

class b2Body;

#include "spriteResource.h"
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class SpriteBuffer {
public:
    void setTexture(unsigned int id, std::size_t spriteId);
    void setModel(unsigned int id, glm::mat4 matrix);
    void setOpacity(unsigned int id, float opacity);

    const unsigned int size() const;

    const float* modelData() const;

    glm::mat4* getModelMatrix(unsigned int id);

    SpriteResource createResource(b2Body& body, unsigned int textureSpriteIndex, bool useLinearScaling = true);
    void         destroyResource(SpriteResource& resource);

    static const int dataSize();
private:
    std::unordered_map<unsigned int, unsigned int> _resourceIds;
    unsigned int                                   _lastResourceId = 0;
    std::vector<float>                             _models;

    glm::mat4* _getModel(unsigned int id);
    float _getTexture(unsigned int id);

    void _setModel(unsigned int id, glm::mat4 model);
    void _setTexture(unsigned int id, float textureId);
    void _setOpacity(unsigned int id, float opacity);
};

#endif