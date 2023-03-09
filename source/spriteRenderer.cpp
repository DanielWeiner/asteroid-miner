#include "spriteRenderer.h"

#include "spriteFactory.h"
#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include "shaderProgramContext.h"

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

layout(std430, binding = 0) readonly buffer uvCoords
{
    vec4 spriteUv[SPRITE_COUNT];
};

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    int idx = int(inSpriteIndex);

    gl_Position = projection * view * inModel * vec4(inPos.xy, 0.0, 1.0);
    TexCoord = vec2(
        spriteUv[idx].x + (inPos.x * spriteUv[idx].z), 
        spriteUv[idx].y + (inPos.y * spriteUv[idx].w)
    );
}  
/*--------------------------------------------*/
)glsl";

const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D spriteSheet;

void main()
{
    FragColor = texture(spriteSheet, TexCoord);
}
/*--------------------------------------------*/
)glsl";

}

SpriteRenderer::SpriteRenderer(
    Window&        window,
    SpriteSheet&   spriteSheet,
    ShaderProgram& shaderProgram,
    bool           useLinearScaling
) : 
_window(window), 
_spriteSheet(spriteSheet), 
_shaderProgram(shaderProgram),
_useLinearScaling(useLinearScaling)
{}

void SpriteRenderer::init(ShaderProgramContext& shaderProgramContext) {
    _view = glm::mat4(1.0);
    _fov = 45.f;
    _texture = _useLinearScaling ? shaderProgramContext.textures[0] : shaderProgramContext.textures[1];
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
    shaderProgram.bindAttributes(instanceBuffer);
    context.instanceVbos.push_back(instanceBuffer);

    return context;
}

Sprite* SpriteRenderer::createSprite(std::string name)
{
    auto spriteId = _spriteBuffer.createResource();
    auto sprite = new Sprite(
        name,
        spriteId, 
        _spriteSheet, 
        _spriteBuffer
    );
    sprite->updateTextureIndex();
    sprite->setScale(sprite->getBaseSize());
    sprite->init();

    return sprite;  
}

void SpriteRenderer::draw() {
    if (_spriteBuffer.size() == 0) {
        return;
    }
    _shaderProgram.use();
    _shaderProgram.bindTexture(_texture);
    _shaderProgram.loadInstanceData(_instanceBuffer, _spriteBuffer.size() * sizeof(float), _spriteBuffer.size() / SpriteBuffer::dataSize(), _spriteBuffer.modelData());
    _shaderProgram.setUniform<int>("spriteSheet", _texture);
    _shaderProgram.setUniform("projection", _projection);
    _shaderProgram.setUniform("view", _view);
    _shaderProgram.drawInstances();
    _shaderProgram.unbindTextures();
    _shaderProgram.unbindVao();
    _spriteBuffer.step();
}