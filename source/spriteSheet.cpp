#include "spriteSheet.h"
#include "global.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath>

namespace {
    const int TRIANGLE_SIZE = 3;
    const int VERTEX_SIZE = 4;

    template<typename...T>
    void concatBuffers(
        std::unique_ptr<const unsigned char[]>& targetBuffer, 
        std::size_t& bufferSize,
        std::vector<T>&... buffers
    ) {
        std::initializer_list<std::pair<unsigned char*, std::size_t>> buffersData{ 
            { 
                (unsigned char*)buffers.data(), 
                buffers.size() * sizeof(T) / sizeof(unsigned char)
            }... 
        };
        int totalSize = 0;
        for (auto& [buffer, size] : buffersData) {
            totalSize += size;
        }

        bufferSize = totalSize;
        unsigned char* newBuffer = new unsigned char[totalSize];
        std::size_t offset = 0;
        for (auto& [buffer, size] : buffersData) {
            std::copy(buffer, buffer + size, newBuffer + offset);
            offset += size;
        }

        targetBuffer.reset(newBuffer);
    }
}

using json = nlohmann::json;

SpriteSheet::SpriteSheet(const char* jsonPath, const char* imagePath)
    : _jsonFile(jsonPath), _imageFile(imagePath)
{
}

void SpriteSheet::load()
{
    auto file = std::ifstream(Global::GetExeDir() + "/" + _jsonFile);
    json data = json::parse(file);
    _width = data["meta"]["size"]["w"];
    _height = data["meta"]["size"]["h"];

    std::vector<float> vertices;
    std::vector<int>   triangles;
    std::vector<int>   vertexOffsets;
    std::vector<int>   vertexCounts;
    std::vector<int>   triangleOffsets;
    std::vector<int>   triangleCounts;

    auto sprites = data["frames"];
    for (auto& [key, value] : sprites.items()) {
        auto triangleCount = value["triangles"].size();
        auto vertexCount = value["vertices"].size();

        _vertexScalarCount += vertexCount * VERTEX_SIZE;
        _triangleScalarCount += triangleCount * TRIANGLE_SIZE;

        _maxVertices = std::max(_maxVertices, 3 * triangleCount);

        vertexOffsets.push_back(vertices.size());
        vertexCounts.push_back(vertexCount);

        triangleOffsets.push_back(triangles.size());
        triangleCounts.push_back(triangleCount);

        float x = value["frame"]["x"];
        float y = value["frame"]["y"];
        float width = value["frame"]["w"];
        float height = value["frame"]["h"];

        auto spriteMatrix = std::make_unique<glm::mat4>(1.0);
        *spriteMatrix = glm::scale(*spriteMatrix, glm::vec3(1 / _width, 1 / _height, 1.0));
        *spriteMatrix = glm::translate(*spriteMatrix, glm::vec3(x, _height - y, 0.0));
        *spriteMatrix = glm::scale(*spriteMatrix, glm::vec3(width, -height, 1.0));

        for (int i = 0; i < value["vertices"].size(); i++) {
            float vertexX = value["vertices"][i][0];
            float vertexY = value["vertices"][i][1];
            float uvX = value["verticesUV"][i][0];
            float uvY = value["verticesUV"][i][1];

            vertices.insert(vertices.end(), {
                vertexX / width,
                vertexY / height,
                uvX / _width,
                uvY / _height
            });
        }

        for (auto& triangle : value["triangles"]) {
            triangles.insert(triangles.end(), {
                (int)triangle[0],
                (int)triangle[1],
                (int)triangle[2]
            });
        }

        _spriteIndices[key] = _spriteCount++;
        _spriteMatrices[key] = std::move(spriteMatrix);
        _dimensions[key] = std::make_unique<glm::vec2>(width, height);
        _locations[key] = glm::vec2(x, _height - y);
    }

    concatBuffers(
        _spriteInfoBuffer, 
        _bufferSize,
        
        triangleOffsets,
        triangleCounts,

        vertexOffsets,
        vertexCounts,
    
        triangles,
        vertices
    );

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

glm::mat4 SpriteSheet::getSpriteMatrix(const char* itemName)
{
    return *_spriteMatrices[itemName];
}

std::size_t SpriteSheet::getSpriteIndex(const char* itemName)
{
    return _spriteIndices[itemName];
}

glm::u8vec4 SpriteSheet::pixelAt(const char* spriteName, float x, float y)
{
    glm::ivec2 location = _locations[spriteName] + glm::ivec2(x, y);

    unsigned char* pixel = _image + (location.x + (int)_width * location.y) * _numChannels * sizeof(unsigned char);
    return glm::u8vec4(pixel[0], pixel[1], pixel[2], pixel[3]);
}

glm::vec2& SpriteSheet::getSize(const char* itemName) {
    return *_dimensions[itemName];
}

glm::vec2 SpriteSheet::getSize()
{
    return glm::vec2(_width, _height);
}

std::size_t SpriteSheet::getMaxVertices() {
    return _maxVertices;
}

std::size_t SpriteSheet::getSpriteCount() {
    return _spriteCount;
}

std::size_t SpriteSheet::getVertexScalarCount() {
    return _vertexScalarCount;
}

std::size_t SpriteSheet::getTriangleScalarCount() {
    return _triangleScalarCount;
}

std::size_t SpriteSheet::getBufferSize()
{
    return _bufferSize;
}

const unsigned char* SpriteSheet::getSpriteInfoBuffer()
{
    return _spriteInfoBuffer.get();
}

unsigned char* SpriteSheet::getRawImage()
{
    return _image;
}

SpriteSheet::~SpriteSheet()
{
    stbi_image_free(_image);
}
