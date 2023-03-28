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
        _spriteGeometries[key] = value;
        auto spriteGeometry = _spriteGeometries[key];

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

unsigned char* SpriteSheet::getRawImage() const
{
    return _image;
}

SpriteSheet::~SpriteSheet()
{
    stbi_image_free(_image);
}


SpriteSheet::SpriteGeometry::SpriteGeometry(const json& json) 
: rotated(json["rotated"]), 
  trimmed(json["trimmed"]),
  frame(json["frame"]),
  spriteSourceSize(json["spriteSourceSize"]),
  sourceSize(json["sourceSize"]),
  vertices([&json](){
    std::vector<glm::ivec2> vec;
    for (auto& item : json["vertices"]) {
        vec.push_back(glm::ivec2(item[0], item[1]));
    }
    return vec;
  }()),
  verticesUv([&json](){
    std::vector<glm::ivec2> vec;
    for (auto& item : json["verticesUV"]) {
        vec.push_back(glm::ivec2(item[0], item[1]));
    }
    return vec;
  }()),
  triangles([&json](){
    std::vector<glm::ivec3> vec;
    for (auto& item : json["triangles"]) {
        vec.push_back(glm::ivec3(item[0], item[1], item[2]));
    }
    return vec;
  }())
{
}

SpriteSheet::SpriteGeometry& SpriteSheet::SpriteGeometry::operator=(json& other)
{
    this->~SpriteGeometry();
    new (this) SpriteGeometry(other);
    return *this;
}

SpriteSheet::SpriteGeometry& SpriteSheet::SpriteGeometry::operator=(SpriteGeometry other)
{
    if (this == &other) return *this;  
    this->~SpriteGeometry(); 
    new (this) SpriteGeometry(other); 
    return *this;  
}

SpriteSheet::SpriteBox::SpriteBox(const json& json) : 
SpriteDimensions(json), x(json["x"]), y(json["y"])
{
}

SpriteSheet::SpriteDimensions::SpriteDimensions(const json& json) : 
width(json["w"]), height(json["h"])
{
}

SpriteSheet::SpriteGeometry::SpriteGeometry(const SpriteGeometry& other)
: rotated(other.rotated),
  trimmed(other.trimmed),
  frame(other.frame),
  spriteSourceSize(other.spriteSourceSize),
  sourceSize(other.sourceSize),
  vertices{other.vertices.begin(), other.vertices.end()},
  verticesUv{other.verticesUv.begin(), other.verticesUv.end()},
  triangles{other.triangles.begin(), other.triangles.end()}
{
}