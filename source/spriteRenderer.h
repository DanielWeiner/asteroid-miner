#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "window.h"
#include "shaderProgram.h"
#include <glm/glm.hpp>

class SpriteRenderer {
public:
    SpriteRenderer(ShaderProgram* shaderProgram, Window* window);
    void init();
    void addSprite(
        const char* spriteName, 
        glm::vec2 position,
        glm::vec2 scale = glm::vec2(1.f),
        float rotate = 0.f
    );
    void draw();
private:
    ShaderProgram*            _shaderProgram;
    std::vector<glm::mat4[2]> _sprites;
    Window* _window;
    SpriteRenderer();
};

#endif