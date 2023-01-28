#include "spriteSheet.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <nlohmann/json.hpp>
#include <iostream>


using json = nlohmann::json;

SpriteSheet::SpriteSheet(const char* jsonPath, const char* imagePath) 
: _jsonFile(jsonPath), _imageFile(imagePath)
{
}

void SpriteSheet::load() 
{
    auto file = std::ifstream(_jsonFile);
    json data = json::parse(file);
    _width = data["meta"]["size"]["w"];
    _height = data["meta"]["size"]["h"];

    auto sprites = data["frames"];
    for (auto& [key, value] : sprites.items()) {
        float x = value["frame"]["x"];
        float y = value["frame"]["y"];
        float width = value["frame"]["w"];
        float height = value["frame"]["h"];

        auto spriteMatrix = std::make_unique<glm::mat4>(1.0);
        *spriteMatrix = glm::scale(*spriteMatrix, glm::vec3(1 / _width, 1 / _height, 1.0));
        *spriteMatrix = glm::translate(*spriteMatrix, glm::vec3(x, _height - y, 0.0));
        *spriteMatrix = glm::scale(*spriteMatrix, glm::vec3(width, -height, 1.0));

        _spriteMatrices[key] = std::move(spriteMatrix);
        _dimensions[key] = std::make_unique<glm::vec2>(width, height);
        _locations[key] = glm::vec2(x, y);
    }

    stbi_set_flip_vertically_on_load(true);
    // load and generate the texture
    int width, height;
    unsigned char *imageData = stbi_load(_imageFile, &width, &height, &_numChannels, 0);
    if (imageData)
    {
        _image = imageData;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}

glm::mat4 SpriteSheet::getSpriteMatrix(const char* itemName)
{
    return *_spriteMatrices[itemName];
}

glm::u8vec4 SpriteSheet::pixelAt(const char* spriteName, float x, float y) 
{
    glm::vec2 relativeLocation = *_spriteMatrices[spriteName] * glm::vec4(x, y, 0.0, 1.0f);
    glm::ivec2 pixelLocation = glm::vec2(relativeLocation.x * _width, relativeLocation.y * _height);

    unsigned char* pixel = _image + (pixelLocation.x + (int)_width * pixelLocation.y) * _numChannels * sizeof(unsigned char);
    return glm::u8vec4(pixel[0], pixel[1], pixel[2], pixel[3]);
}

glm::vec2& SpriteSheet::getRawDimensions(const char* itemName) { 
    return *_dimensions[itemName];
}

glm::vec2 SpriteSheet::getRawDimensions()
{
    return glm::vec2(_width, _height);
}

unsigned char* SpriteSheet::getRawImage()
{
    return _image;
}

SpriteSheet::~SpriteSheet() 
{
    stbi_image_free(_image);
}
