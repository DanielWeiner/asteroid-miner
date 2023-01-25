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
    SpriteRenderer(ShaderProgram* shaderProgram, Window* window);
    void init();
    std::shared_ptr<Sprite> createSprite(const char* spriteName);
    void addSprite(std::shared_ptr<Sprite> sprite);
    void draw();
private:
    ShaderProgram*                       _shaderProgram;
    std::vector<std::shared_ptr<Sprite>> _sprites;
    Window* _window;

    SpriteRenderer();
};

#endif