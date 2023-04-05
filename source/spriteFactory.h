#ifndef SPRITE_FACTORY_H_
#define SPRITE_FACTORY_H_

#include "shaderProgramContext.h"
#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include "window.h"
#include "spriteRenderer.h"

#include <string>
#include <vector>
#include <box2d/b2_world.h>

class SpriteFactory {
public:
    SpriteFactory(
        const Window& window, 
        SpriteSheet& spriteSheet
    );

    SpriteRenderer* createRenderer(b2World& world, Sprite::EventListener* eventListener = new Sprite::DefaultEventListener());
private:
    const Window&      _window;
    SpriteSheet&       _spriteSheet;
    std::vector<float> _spriteInfoBuffer;

    SpriteRenderer::ShaderProgramData _shaderProgramData{
        .spriteSheet      = _spriteSheet,
        .spriteInfoBuffer = _spriteInfoBuffer
    };

    ShaderProgramContext              _shaderProgramContext;
    ShaderProgram                     _shaderProgram;

    bool               _initialized = false;

    void _init();
};

#endif