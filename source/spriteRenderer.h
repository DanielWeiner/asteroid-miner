#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "sprite.h"
#include "shaderProgram.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class SpriteBuffer;

class SpriteRenderer {
public:
    SpriteRenderer(
        std::shared_ptr<SpriteSheet> spriteSheet,
        std::shared_ptr<SpriteBuffer> spriteBuffer,
        float width,
        float height
    );

    void init();
    void updateDimensions(float width, float height);
    
    void draw();
    void clearScreen();
private:
    std::unique_ptr<ShaderProgram>                     _shaderProgram;
    std::shared_ptr<SpriteSheet>                       _spriteSheet;
    std::shared_ptr<SpriteBuffer>                      _spriteBuffer;
    float                                              _width;
    float                                              _height;
    unsigned int                                       _vbo1;
    unsigned int                                       _vbo2;
    bool                                               _spritesDirty = false;
    unsigned int                                       _lastSpriteId = 0;

    friend Sprite::~Sprite();

};

#endif