#ifndef SPRITE_BUFFER_H_
#define SPRITE_BUFFER_H_

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

class SpriteBuffer {
public:
    void rotateModel(unsigned int id, float radians, glm::vec3 point);
    void scaleModel(unsigned int id, glm::vec3 point);
    void translateModel(unsigned int id, glm::vec3 point);

    void setTexture(unsigned int id, glm::mat4 matrix);
    void initializeModel(unsigned int id, glm::mat4&& matrix);

    bool areTexturesDirty();
    void resetDirtyFlag();

    unsigned int size();

    glm::mat4* modelData();
    glm::mat4* textureData();

    glm::mat4 getModelMatrix(unsigned int id);

    unsigned int createResource();
    void destroyResource(unsigned int id);
private:
    std::unordered_map<unsigned int, unsigned int> _resourceIds;
    bool                                           _texturesDirty = false;
    unsigned int                                   _lastResourceId = 0;
    std::unique_ptr<glm::mat4[]>                   _models;
    std::unique_ptr<glm::mat4[]>                   _textures;

    int                                            _capacity = 1;
    unsigned int                                   _length = 0;
};

#endif