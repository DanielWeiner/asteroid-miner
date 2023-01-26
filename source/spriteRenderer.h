#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "window.h"
#include "shaderProgram.h"
#include "sprite.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class SpriteRenderer {
public:
    SpriteRenderer();
    SpriteRenderer(float width, float height);
    void init();
    void setBuffer(const unsigned int size);
    void updateDimensions(float width, float height);
    std::unique_ptr<Sprite> createSprite(const char* spriteName);
    void draw();
    void clearScreen();
private:
    ShaderProgram                        _shaderProgram;
    std::vector<std::shared_ptr<Sprite>> _sprites;
    float                                _width;
    float                                _height;
    unsigned int                         _bufSize;
    std::unique_ptr<glm::mat4[]>         _buffer;
    std::unique_ptr<glm::mat4[]>         _texBuffer;
    unsigned int                         _length = 0;
    unsigned int                         _vbo1;
    unsigned int                         _vbo2;
    bool                                 _spritesDirty = false;
};

#endif