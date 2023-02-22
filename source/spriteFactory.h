#ifndef SPRITE_FACTORY_H_
#define SPRITE_FACTORY_H_

#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include "window.h"
#include "spriteRenderer.h"

#include <memory>
#include <string>

class SpriteFactory {
public:
    SpriteFactory(
        std::shared_ptr<SpriteSheet>   spriteSheet,
        std::shared_ptr<Window>        window
    );

    std::unique_ptr<Sprite> createSprite(std::string name);
    std::unique_ptr<SpriteRenderer> createRenderer(bool useLinearScaling = false);
private:
    std::shared_ptr<SpriteSheet>   _spriteSheet;
    std::shared_ptr<SpriteBuffer>  _spriteBuffer;
    std::shared_ptr<Window>        _window;
};

#endif