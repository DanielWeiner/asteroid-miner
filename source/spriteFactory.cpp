#include "spriteFactory.h"

SpriteFactory::SpriteFactory(
    Window& window,
    SpriteSheet& spriteSheet
) : 
_window(window),
_spriteSheet(spriteSheet)
{}

SpriteRenderer* SpriteFactory::createRenderer(bool useLinearScaling) {
    _init();

    auto renderer = new SpriteRenderer(
        _window, 
        _spriteSheet, 
        _shaderProgram, 
        useLinearScaling
    );
    renderer->init(_shaderProgramContext);
    
    return renderer;
}

void SpriteFactory::_init() 
{
    if (_initialized) {
        return;
    }

    _initialized = true;
    _spriteSheet.load(_spriteInfoBuffer);
    _shaderProgramContext = _shaderProgram.init(SpriteRenderer::initializeShaderProgram, &_shaderProgramData);
}
