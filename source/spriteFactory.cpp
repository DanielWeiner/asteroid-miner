#include "spriteFactory.h"

SpriteFactory::SpriteFactory(
    std::shared_ptr<SpriteSheet> spriteSheet,
    std::shared_ptr<SpriteBuffer> spriteBuffer,
    std::shared_ptr<Window>       window
)
: 
_spriteSheet(spriteSheet),
_spriteBuffer(spriteBuffer),
_window(window)
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

std::unique_ptr<SpriteRenderer> SpriteFactory::createRenderer()
{
    return std::make_unique<SpriteRenderer>(_window, _spriteSheet, _spriteBuffer);
}
