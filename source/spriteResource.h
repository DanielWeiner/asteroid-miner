#ifndef SPRITE_RESOURCE_H_
#define SPRITE_RESOURCE_H_

#include <cstddef>
#include <functional>
#include <box2d/b2_body.h>

struct SpriteResource {
    const unsigned int id;             // id of the resource
    b2Body&            body;           // body of the sprite

    bool operator==(const SpriteResource& other) const;
    bool operator!=(const SpriteResource& other) const;
    bool operator<(const SpriteResource& other) const;
};

template <>
struct std::hash<SpriteResource> {
    std::size_t operator()(const SpriteResource& resource) const {
        return std::hash<unsigned int>()(resource.id);
    }
};

#endif