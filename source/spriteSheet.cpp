#include "spriteSheet.h"
#include "global.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <cmath>


SpriteSheet::SpriteSheet(const char* jsonPath, const char* imagePath)
    : _jsonFile(jsonPath), _imageFile(imagePath)
{
}

void SpriteSheet::load(std::vector<float>& spriteVertices)
{
    auto file = std::ifstream(Global::GetExeDir() + "/" + _jsonFile);
    json data = json::parse(file);
    _width = data["meta"]["size"]["w"];
    _height = data["meta"]["size"]["h"];

    auto sprites = data["frames"];

    int spriteCount = 0;
    for (auto& [key, value] : sprites.items()) {
        _spriteGeometries.emplace(key, value);
        auto& spriteGeometry = _spriteGeometries.at(key);

        float x = spriteGeometry.frame.x;
        float y = spriteGeometry.frame.y;
        float width = spriteGeometry.frame.width;
        float height = spriteGeometry.frame.height;
        
        spriteVertices.insert(spriteVertices.end(), {
            x / _width,
            y / _height,
            width / _width,
            height / _height
        });

        _spriteIndices[key] = spriteCount++;
    }

    // load and generate the texture
    int width, height;
    unsigned char* imageData = stbi_load((Global::GetExeDir() + "/" + _imageFile).c_str(), &width, &height, &_numChannels, 0);
    if (imageData)
    {
        _image = imageData;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}

std::size_t SpriteSheet::getSpriteIndex(const char* itemName) const
{
    return _spriteIndices.at(itemName);
}

glm::u8vec4 SpriteSheet::pixelAt(const char* spriteName, float x, float y) const
{
    auto geometry = _spriteGeometries.at(spriteName);
    glm::ivec2 location(geometry.frame.x, geometry.frame.y);

    unsigned char* pixel = _image + (location.x + (int)_width * location.y) * _numChannels * sizeof(unsigned char);
    return glm::u8vec4(pixel[0], pixel[1], pixel[2], pixel[3]);
}

glm::vec2 SpriteSheet::getSize(const char* itemName) const 
{
    auto geometry = _spriteGeometries.at(itemName);
    return glm::vec2(geometry.sourceSize.width, geometry.sourceSize.height);
}

glm::vec2 SpriteSheet::getSize() const
{
    return glm::vec2(_width, _height);
}

const SpriteGeometry& SpriteSheet::getSpriteGeometry(const char* itemName) const
{
    return _spriteGeometries.at(itemName);
}

unsigned char* SpriteSheet::getRawImage() const
{
    return _image;
}

SpriteSheet::~SpriteSheet()
{
    stbi_image_free(_image);
}