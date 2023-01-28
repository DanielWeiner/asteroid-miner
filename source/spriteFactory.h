#ifndef SPRITE_FACTORY_H_
#define SPRITE_FACTORY_H_

#include <memory>
#include <string>

class SpriteRenderer;
class SpriteSheet;
class Sprite;
class SpriteBuffer;

class SpriteFactory {
public:
    SpriteFactory(std::shared_ptr<SpriteSheet> spriteSheet, std::shared_ptr<SpriteBuffer> spriteBuffer);

    std::unique_ptr<Sprite> createSprite(std::string name);
    std::unique_ptr<SpriteRenderer> createRenderer(float width, float height);
private:
    std::shared_ptr<SpriteSheet>    _spriteSheet;
    std::shared_ptr<SpriteBuffer>   _spriteBuffer;
};

#endif