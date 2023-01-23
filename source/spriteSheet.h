#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <glm/vec4.hpp>

#include <string>
#include <unordered_map>
#include <vector>



class SpriteSheet {
public:
    struct BoundingBox {
        float x;
        float y;
        float width;
        float height;
    };
    using SpriteMap = std::unordered_map<std::string, BoundingBox>;

    SpriteSheet(const char* path);
    void load();
    BoundingBox getSpriteBoundingBox(const char* itemName);
private:
    const char* _file;
    int         _width;
    int         _height;
                                         
    SpriteMap _sprites;
};

#endif