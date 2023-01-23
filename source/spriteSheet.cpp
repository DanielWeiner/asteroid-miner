#include "spriteSheet.h"

#include <fstream>
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
        _sprites[key] = {
            .x = value["frame"]["x"],
            .y = value["frame"]["y"],
            .width = value["frame"]["w"],
            .height = value["frame"]["h"]
        };
    }
}

SpriteSheet::BoundingBox SpriteSheet::getSpriteBoundingBox(const char* itemName)
{
    auto box = _sprites[itemName];

    return {
        .x = box.x / _width,
        .y = (_height - box.y) / _height,
        .width = box.width / _width,
        .height = box.height / _height
    };
}
