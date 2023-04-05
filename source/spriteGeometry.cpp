#include "spriteGeometry.h"

#include <unordered_map>
#include <unordered_set>

#include <string>
#include <box2d/b2_polygon_shape.h>

namespace {
    template<int N>
    using Vec = glm::vec<N, int>;

    template<int N>
    using VecArray = std::vector<Vec<N>>;

    template<int N>
    const VecArray<N> extractVectors(const Json& json, const char* key) {
        VecArray<N> result(json[key].size());

        int i{0};
        for (auto& item : json[key]) {
            Vec<N> vec;
            
            for (int j{0}; j < N; j++) {
                vec[j] = item[j];
            }

            result[i++] = vec;
        }

        return result;
    }
}

SpriteGeometry::SpriteGeometry(const Json& json)
    : rotated(json["rotated"]),
      trimmed(json["trimmed"]),
      frame({ 
        .x = json["frame"]["x"],
        .y = json["frame"]["y"],
        .width = json["frame"]["w"],
        .height = json["frame"]["h"] 
      }),
      spriteSourceSize({ 
        .x = json["spriteSourceSize"]["x"],
        .y = json["spriteSourceSize"]["y"],
        .width = json["spriteSourceSize"]["w"],
        .height = json["spriteSourceSize"]["h"] 
      }),
      sourceSize({ 
        .width = json["sourceSize"]["w"],
        .height = json["sourceSize"]["h"]
      }),
      vertices(extractVectors<2>(json, "vertices")),
      verticesUv(extractVectors<2>(json, "verticesUV")),
      triangles(extractVectors<3>(json, "triangles"))
{
}

const std::vector<std::array<glm::vec2, 3>> SpriteGeometry::getTriangles() const {
    std::vector<std::array<glm::vec2, 3>> results;

    for (auto& triangle : triangles) {
        std::array<glm::vec2, 3> points;

        for (int i{0}; i < 3; i++) {
            points[i] = glm::vec2(
                (float)vertices[triangle[i]].x / (float)sourceSize.width - 0.5f,
                (float)vertices[triangle[i]].y / (float)sourceSize.height - 0.5f
            );
        }

        results.push_back(points);
    }

    return results;
}
