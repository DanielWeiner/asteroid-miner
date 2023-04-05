#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <glm/glm.hpp>

#include "spriteGeometry.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SpriteSheet {
public:

    using SpriteGeometryMap = std::unordered_map<std::string, SpriteGeometry>;
    using SpriteIndexMap = std::unordered_map<std::string, int>;

    SpriteSheet(const char* jsonPath, const char* imagePath);
    void load(std::vector<float>& vertices);
    std::size_t getSpriteIndex(const char* itemName) const;
    glm::u8vec4 pixelAt(const char* spriteName, float x, float y) const;
    glm::vec2 getSize(const char* itemName) const;
    glm::vec2 getSize() const;
    const SpriteGeometry& getSpriteGeometry(const char* itemName) const;

    unsigned char* getRawImage() const;

    ~SpriteSheet();
private:
    using SpriteGeometryMap = std::unordered_map<std::string, SpriteGeometry>;

    const char*       _jsonFile;
    const char*       _imageFile;
    float             _width;
    float             _height;
    unsigned char*    _image = NULL;
    int               _numChannels;
    SpriteIndexMap    _spriteIndices;
    SpriteGeometryMap _spriteGeometries;
};

#endif