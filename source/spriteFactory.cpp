#include "spriteFactory.h"

SpriteFactory::SpriteFactory(
    std::shared_ptr<SpriteSheet>   spriteSheet,
    std::shared_ptr<Window>        window
)
: 
_spriteSheet(spriteSheet),
_window(window)
{}

std::unique_ptr<Sprite> SpriteFactory::createSprite(std::string name)
{
    if (!_spriteBuffer) {
        _spriteBuffer = std::make_unique<SpriteBuffer>();
    }

    auto spriteId = _spriteBuffer->createResource();
    auto sprite = std::make_unique<Sprite>(
        name,
        spriteId, 
        _spriteSheet, 
        _spriteBuffer
    );
    sprite->updateTextureMatrix();
    sprite->scaleBy(_spriteSheet->getSize(name.c_str()));
    sprite->init();

    return std::move(sprite);
}

std::unique_ptr<SpriteRenderer>
SpriteFactory::createRenderer(bool useLinearScaling) {
    if (!_spriteBuffer) {
        _spriteBuffer = std::make_shared<SpriteBuffer>();
    }

    auto shaderProgram = std::make_shared<ShaderProgram>();
    shaderProgram->init();

    auto renderer = std::make_unique<SpriteRenderer>(_window, _spriteSheet, _spriteBuffer, shaderProgram, useLinearScaling);
    renderer->init();
    
    return std::move(renderer);
}
