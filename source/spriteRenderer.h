#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "shaderProgram.h"


class SpriteRenderer {
public:
    SpriteRenderer(const ShaderProgram& shaderProgram);
    void init();
    void draw();
private:
    ShaderProgram _shaderProgram;
    SpriteRenderer();
};

#endif