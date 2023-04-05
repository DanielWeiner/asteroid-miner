#include "spriteResource.h"

bool SpriteResource::operator==(const SpriteResource& other) const
{
    return id == other.id;
}

bool SpriteResource::operator!=(const SpriteResource& other) const
{
    return !(*this == other);
}

bool SpriteResource::operator<(const SpriteResource& other) const
{
    return id < other.id;
}
