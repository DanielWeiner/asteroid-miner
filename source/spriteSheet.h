#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class SpriteSheet {
public:
    using SpriteMap = std::unordered_map<std::string, std::unique_ptr<glm::mat4>>;
    using DimensionMap = std::unordered_map<std::string, std::unique_ptr<glm::vec2>>;

    SpriteSheet(const char* path);
    void load();
    void writeSpriteMatrix(const char* itemName, glm::mat4* buffer);
    glm::vec2& getRawDimensions(const char* itemName);
private:
    const char* _file;
    float       _width;
    float       _height;
                                         
    SpriteMap    _spriteMatrices;
    DimensionMap _dimensions;
};

#endif