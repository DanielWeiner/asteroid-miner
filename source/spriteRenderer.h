#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "sprite.h"
#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "window.h"
#include <box2d/b2_world.h>
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
        const Window&          window,
        const SpriteSheet&     spriteSheet,
        ShaderProgram&         shaderProgram,
        b2World&               world,
        Sprite::EventListener* onUpdate = new Sprite::DefaultEventListener()
    );

    void init(const ShaderProgramContext& shaderProgramData);
    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);

    static ShaderProgramContext initializeShaderProgram(ShaderProgram& shaderProgram, void* additionalData);

    Sprite* createSprite(std::string name, bool useLinearScaling = true, bool enableCollisions = true);
    
    void draw();
private:
    const Window&           _window;
    const SpriteSheet&      _spriteSheet;
    ShaderProgram&          _shaderProgram;
    b2World&                _world;
    Sprite::EventListener*  _spriteEventListener;
    SpriteBuffer            _spriteBuffer;
    unsigned int            _instanceBuffer;
    GLuint                  _texture;
    GLuint                  _linearTexture;
    float                   _fov;
    glm::mat4               _view;
    glm::mat4               _projection;
    bool                    _useLinearScaling;

    friend Sprite::~Sprite();
};

#endif