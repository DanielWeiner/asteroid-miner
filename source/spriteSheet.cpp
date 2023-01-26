#include "spriteSheet.h"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

SpriteSheet::SpriteSheet(const char *path) 
{
    _file = path;
}

void SpriteSheet::load() 
{
    auto file = std::ifstream(_file);
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
    }
}

void SpriteSheet::writeSpriteMatrix(const char* itemName, glm::mat4* buf) { 
    *buf = *_spriteMatrices[itemName];
}

glm::vec2& SpriteSheet::getRawDimensions(const char* itemName) { 
    return *_dimensions[itemName];
}