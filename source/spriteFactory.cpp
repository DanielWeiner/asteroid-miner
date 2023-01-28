#include "spriteFactory.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include "spriteRenderer.h"


SpriteFactory::SpriteFactory(
    std::shared_ptr<SpriteSheet> spriteSheet,
    std::shared_ptr<SpriteBuffer> spriteBuffer
)
: 
_spriteSheet(spriteSheet),
_spriteBuffer(spriteBuffer)
{}

std::unique_ptr<Sprite> SpriteFactory::createSprite(std::string name)
{
    auto spriteId = _spriteBuffer->createResource();
    auto sprite = std::make_unique<Sprite>(
        name,
        spriteId, 
        _spriteSheet, 
        _spriteBuffer
    );
    sprite->updateTextureMatrix();
    sprite->scaleBy(_spriteSheet->getRawDimensions(name.c_str()));

    return std::move(sprite);
}

std::unique_ptr<SpriteRenderer> SpriteFactory::createRenderer(float width, float height)
{
    return std::make_unique<SpriteRenderer>(_spriteSheet, _spriteBuffer, width, height);
}
