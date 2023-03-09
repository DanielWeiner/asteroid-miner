#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class SpriteSheet {
public:
    using SpriteIndexMap = std::unordered_map<std::string, int>;

    SpriteSheet(const char* jsonPath, const char* imagePath);
    void load(std::vector<float>& vertices);
    std::size_t getSpriteIndex(const char* itemName);
    glm::u8vec4 pixelAt(const char* spriteName, float x, float y);
    glm::vec2 getSize(const char* itemName);
    glm::vec2 getSize();

    unsigned char* getRawImage();

    ~SpriteSheet();
private:
    struct SpriteDimensions {
        SpriteDimensions(const json& json);

        const int width;
        const int height;
    };

    struct SpriteBox : SpriteDimensions {
        SpriteBox(const json& json);

        const int x;
        const int y;
    };

    struct SpriteGeometry {
        SpriteGeometry() = default;
        SpriteGeometry(const SpriteGeometry& other);
        SpriteGeometry(const json& json);
        
        const bool                    rotated{false};
        const bool                    trimmed{false};
        const SpriteBox               frame{{ {"x", 0}, {"y", 0}, {"w", 0}, {"h", 0} }};
        const SpriteBox               spriteSourceSize{{ {"x", 0},  {"y", 0}, {"w", 0}, {"h", 0} }};
        const SpriteDimensions        sourceSize{{ {"w", 0}, {"h", 0} }};
        const std::vector<glm::ivec2> vertices{};
        const std::vector<glm::ivec2> verticesUv{};
        const std::vector<glm::ivec3> triangles{};

        SpriteGeometry& operator=(json& other);
        SpriteGeometry& operator=(SpriteGeometry other);
    };

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