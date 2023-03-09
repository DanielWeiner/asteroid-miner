#ifndef SPRITE_BUFFER_H_
#define SPRITE_BUFFER_H_

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

class SpriteBuffer {
public:
    void setTexture(unsigned int id, std::size_t spriteId);
    void setModel(unsigned int id, glm::mat4 matrix);

    unsigned int size();

    float* modelData();

    glm::mat4* getModelMatrix(unsigned int id);

    unsigned int createResource();
    void destroyResource(unsigned int id);
    void moveToEnd(unsigned int id);

    unsigned int getStep();
    unsigned int getNextStep();

    void step();

    static std::size_t dataSize();
private:
    std::unordered_map<unsigned int, unsigned int> _resourceIds;
    unsigned int                                   _lastResourceId = 0;
    std::vector<float>                             _models;
    unsigned int                                   _step = 0;

    glm::mat4* _getModel(unsigned int id);
    float _getTexture(unsigned int id);

    void _setModel(unsigned int id, glm::mat4 model);
    void _setTexture(unsigned int id, float textureId);
};

#endif