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
    using LocationMap = std::unordered_map<std::string, glm::vec2>;

    SpriteSheet(const char* jsonPath, const char* imagePath);
    void load();
    glm::mat4 getSpriteMatrix(const char* itemName);
    glm::u8vec4 pixelAt(const char* spriteName, float x, float y);
    glm::vec2& getSize(const char* itemName);
    glm::vec2 getSize();

    unsigned char* getRawImage();

    ~SpriteSheet();
private:
    const char*    _jsonFile;
    const char*    _imageFile;
    float          _width;
    float          _height;
    unsigned char* _image = NULL;
    int            _numChannels;

    SpriteMap    _spriteMatrices;
    DimensionMap _dimensions;
    LocationMap  _locations;
};

#endif