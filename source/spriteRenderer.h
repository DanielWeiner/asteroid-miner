#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "sprite.h"
#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "window.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

class Window;
class SpriteFactory;
class ShaderProgramContext;

class SpriteRenderer {
public:
    struct ShaderProgramData {
        SpriteSheet&        spriteSheet;
        std::vector<float>& spriteInfoBuffer;
    };

    SpriteRenderer(
        Window& window,
        SpriteSheet& spriteSheet,
        ShaderProgram& shaderProgram,
        bool useLinearScaling = true
    );

    void init(ShaderProgramContext& shaderProgramData);
    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);

    static ShaderProgramContext initializeShaderProgram(ShaderProgram& shaderProgram, void* additionalData);

    Sprite* createSprite(std::string name);
    
    void draw();
private:
    Window&        _window;
    SpriteSheet&   _spriteSheet;
    SpriteBuffer   _spriteBuffer;
    ShaderProgram& _shaderProgram;
    unsigned int   _instanceBuffer;
    unsigned int   _lastSpriteId = 0;
    GLuint         _texture;
    float          _fov;
    glm::mat4      _view;
    glm::mat4      _projection;
    bool           _useLinearScaling;
    friend Sprite::~Sprite();

};

#endif