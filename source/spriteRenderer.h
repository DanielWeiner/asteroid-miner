#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "sprite.h"
#include "shaderProgram.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class SpriteBuffer;
class Window;

class SpriteRenderer {
public:
    SpriteRenderer(
        std::shared_ptr<Window> window,
        std::shared_ptr<SpriteSheet> spriteSheet,
        std::shared_ptr<SpriteBuffer> spriteBuffer,
        std::shared_ptr<ShaderProgram> shaderProgram
    );

    void init();
    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);
    
    void draw();
private:
    std::shared_ptr<Window>        _window;
    std::shared_ptr<SpriteSheet>   _spriteSheet;
    std::shared_ptr<SpriteBuffer>  _spriteBuffer;
    std::shared_ptr<ShaderProgram> _shaderProgram;
    unsigned int                   _vbo1;
    unsigned int                   _vbo2;
    bool                           _spritesDirty = false;
    unsigned int                   _lastSpriteId = 0;
    GLuint                         _texture;
    float                          _fov;
    glm::mat4                      _view;
    glm::mat4                      _projection;
    friend Sprite::~Sprite();

};

#endif