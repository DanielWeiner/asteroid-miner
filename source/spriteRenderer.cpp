#include "spriteRenderer.h"
#include "constants.h"
#include "spriteFactory.h"
#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include "shaderProgramContext.h"

#include <box2d/b2_body.h>
#include <box2d/b2_mouse_joint.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <string>

namespace  {
static constexpr float spriteRect[]{
    0.f, 1.f,
    0.f, 0.f,
    1.f, 0.f,

    1.f, 1.f,
    0.f, 1.f,
    1.f, 0.f
};


template<typename T>
std::string replace(std::string source, std::string find, T replace) {
    std::string replaceStr = std::to_string(replace);
    std::size_t index = source.find(find);
    while (index != std::string::npos) {
        source.replace(index, find.size(), replaceStr);
        index += replaceStr.size();
        index = source.find(find);
    }
    return source;
}

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 430 core

layout (location = 0) in vec2  inPos;
layout (location = 1) in mat4  inModel;
layout (location = 5) in float inSpriteIndex;
layout (location = 6) in float inSpriteOpacity;
layout (location = 7) in float inSpriteUseLinearScaling;

layout(std430, binding = 0) readonly buffer uvCoords
{
    vec4 spriteUv[SPRITE_COUNT];
};

out vec2 texCoord;
out float opacity;
out float useLinearScaling;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    int idx = int(inSpriteIndex);

    gl_Position = projection * view * inModel * vec4(inPos.xy, 0.0, 1.0);
    texCoord = vec2(
        spriteUv[idx].x + (inPos.x * spriteUv[idx].z), 
        spriteUv[idx].y + (inPos.y * spriteUv[idx].w)
    );
    opacity = inSpriteOpacity;
    useLinearScaling = inSpriteUseLinearScaling;
}  
/*--------------------------------------------*/
)glsl";

const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 430 core
out vec4 color;

in vec2 texCoord;
in float opacity;
in float useLinearScaling;

uniform sampler2D spriteSheet;
uniform sampler2D linearSpriteSheet;

void main()
{
    if (useLinearScaling == 1.0) {
        color = texture(linearSpriteSheet, texCoord);
    } else {
        color = texture(spriteSheet, texCoord);
    }
    color.a *= opacity;
}
/*--------------------------------------------*/
)glsl";

}

SpriteRenderer::SpriteRenderer(
    const Window&          window,
    const SpriteSheet&     spriteSheet,
    ShaderProgram&         shaderProgram,
    b2World&               world,
    Sprite::EventListener* spriteEventListener
) :  
_window(window), 
_spriteSheet(spriteSheet), 
_shaderProgram(shaderProgram),
_world(world),
_spriteEventListener(spriteEventListener)
{}

void SpriteRenderer::init(const ShaderProgramContext& shaderProgramContext) {
    _view = glm::mat4(1.0);
    _fov = 45.f;
    _linearTexture = shaderProgramContext.textures[0];
    _texture = shaderProgramContext.textures[1];
    _instanceBuffer = shaderProgramContext.instanceVbos[0];
}

void SpriteRenderer::setView(glm::mat4 view) 
{
    _view = view;
}

void SpriteRenderer::setProjection(glm::mat4 projection) 
{
    _projection = projection;
}

ShaderProgramContext SpriteRenderer::initializeShaderProgram(ShaderProgram& shaderProgram, void* additionalData)
{
    ShaderProgramContext context;
    ShaderProgramData* shaderProgramData = (ShaderProgramData*)additionalData;
    auto& spriteSheet = shaderProgramData->spriteSheet;
    auto& spriteInfoBuffer = shaderProgramData->spriteInfoBuffer;

    auto sheetSize = spriteSheet.getSize();

    std::string vertexShaderStr(vertexShaderSource);
    vertexShaderStr = replace(vertexShaderStr, "SPRITE_COUNT", spriteInfoBuffer.size() / 4);

    context.textures.push_back(shaderProgram.loadTexture(spriteSheet.getRawImage(), sheetSize.x, sheetSize.y, 4, true));
    context.textures.push_back(shaderProgram.loadTexture(spriteSheet.getRawImage(), sheetSize.x, sheetSize.y, 4, false));

    shaderProgram.addVertexShader(vertexShaderStr.c_str());
    shaderProgram.addFragmentShader(fragmentShaderSource);
    shaderProgram.linkShaders();

    shaderProgram.initShaderStorageBuffer();
    shaderProgram.loadShaderStorageData(
       0, 
        spriteInfoBuffer.size() * sizeof(float), 
        spriteInfoBuffer.data()
    );

    shaderProgram.loadData(spriteRect);
    shaderProgram.defineAttribute<float>("inPos", 2);
    shaderProgram.bindAttributes();

    auto instanceBuffer = shaderProgram.initInstanceBuffer();
    shaderProgram.defineInstanceAttribute<glm::vec4>(instanceBuffer, "inModel", 4);
    shaderProgram.defineInstanceAttribute<float>(instanceBuffer, "inSpriteIndex", 1);
    shaderProgram.defineInstanceAttribute<float>(instanceBuffer, "inSpriteOpacity", 1);
    shaderProgram.defineInstanceAttribute<float>(instanceBuffer, "inSpriteUseLinearScaling", 1);
    shaderProgram.bindAttributes(instanceBuffer);
    context.instanceVbos.push_back(instanceBuffer);

    return context;
}

Sprite* SpriteRenderer::createSprite(std::string name, bool useLinearScaling, bool enableCollisions)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.enabled = true;
    bodyDef.awake = true;

    auto sprite = new Sprite(
        name,
        _spriteSheet,
        _world.CreateBody(&bodyDef),
        _spriteBuffer,
        useLinearScaling,
        enableCollisions,
        _spriteEventListener
    );
    sprite->setOpacity(1.0);

    return sprite;  
}

void SpriteRenderer::draw() {
    if (_spriteBuffer.size() == 0) {
        return;
    }
    _shaderProgram.use();
    _shaderProgram.bindTexture(_texture);
    _shaderProgram.bindTexture(_linearTexture);
    _shaderProgram.loadInstanceData(_instanceBuffer, _spriteBuffer.size() * sizeof(float), _spriteBuffer.size() / SpriteBuffer::dataSize(), _spriteBuffer.modelData());
    _shaderProgram.setUniform<int>("spriteSheet", _texture);
    _shaderProgram.setUniform<int>("linearSpriteSheet", _linearTexture);
    _shaderProgram.setUniform("projection", _projection);
    _shaderProgram.setUniform("view", _view);
    _shaderProgram.drawInstances();
    _shaderProgram.unbindTextures();
    _shaderProgram.unbindVao();
}