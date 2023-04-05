#ifndef SPRITE_GEOMETRY_H_
#define SPRITE_GEOMETRY_H_

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>

using Json = nlohmann::json;

class SpriteGeometry {
    struct SpriteDimensions {
        const int width{-1};
        const int height{1};
    };

    struct SpriteBox {
        const int x{-1};
        const int y{-1};
        const int width{-1};
        const int height{1};
    };

public:
    SpriteGeometry() = default;
    SpriteGeometry(const Json& json);

    const bool                        rotated{false};
    const bool                        trimmed{false};
    const SpriteBox                   frame{{}};
    const SpriteBox                   spriteSourceSize{{}};
    const SpriteDimensions            sourceSize{{}};
    const std::vector<glm::ivec2>     vertices{};
    const std::vector<glm::ivec2>     verticesUv{};
    const std::vector<glm::ivec3>     triangles{};

    const std::vector<std::array<glm::vec2, 3>> getTriangles() const;
};

#endif