#ifndef SPRITE_SHEET_H_
#define SPRITE_SHEET_H_

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <array>
class SpriteSheet {
public:
    using SpriteIndexMap = std::unordered_map<std::string, int>;
    using SpriteMap = std::unordered_map<std::string, std::unique_ptr<glm::mat4>>;
    using DimensionMap = std::unordered_map<std::string, std::unique_ptr<glm::vec2>>;
    using LocationMap = std::unordered_map<std::string, glm::ivec2>;

    SpriteSheet(const char* jsonPath, const char* imagePath);
    void load();
    glm::mat4 getSpriteMatrix(const char* itemName);
    std::size_t getSpriteIndex(const char* itemName);
    glm::u8vec4 pixelAt(const char* spriteName, float x, float y);
    glm::vec2& getSize(const char* itemName);
    glm::vec2 getSize();

    std::size_t getMaxVertices();
    std::size_t getSpriteCount();
    std::size_t getVertexScalarCount();
    std::size_t getTriangleScalarCount();
    std::size_t getBufferSize();
    const unsigned char* getSpriteInfoBuffer();

    unsigned char* getRawImage();

    ~SpriteSheet();
private:
    struct SpriteDimensions {
        int x;
        int y;
    };

    struct SpriteBox : SpriteDimensions {
        int width;
        int height;
    };

    struct SpriteDefinition {
    friend SpriteSheet;
    private:
        std::string                     name;
        bool                            rotated;
        bool                            trimmed;
        SpriteBox                       frame;
        SpriteBox                       spriteSourceSize;
        SpriteDimensions                sourceSize;
        std::vector<std::array<int, 2>> vertices;
        std::vector<std::array<int, 3>> verticesUv;
        std::vector<std::array<int, 3>> triangles;
    };

    const char*                            _jsonFile;
    const char*                            _imageFile;
    float                                  _width;
    float                                  _height;
    unsigned char*                         _image = NULL;
    int                                    _numChannels;
    std::size_t                            _spriteCount = 0;
    std::size_t                            _vertexScalarCount = 0;
    std::size_t                            _triangleScalarCount = 0;
    std::size_t                            _maxVertices = 0;
    std::size_t                            _bufferSize = 0;

    std::unique_ptr<const unsigned char[]> _spriteInfoBuffer;

    SpriteMap      _spriteMatrices;
    DimensionMap   _dimensions;
    LocationMap    _locations;
    SpriteIndexMap _spriteIndices;
};

#endif