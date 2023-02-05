#include "spriteRenderer.h"

#include "shaderProgram.h"
#include "spriteBuffer.h"
#include "sprite.h"
#include "spriteSheet.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <exception>
#include <iostream>
#include "window.h"

namespace  {

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in mat4 instanceModel;
layout (location = 5) in mat4 instanceTexModel;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * instanceModel * vec4(aPos.xy, 0.0, 1.0);
    TexCoord = vec2(instanceTexModel * vec4(aPos.xy, 0.0, 1.0));
}  
/*--------------------------------------------*/
)glsl";

const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 330 core
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
    std::shared_ptr<Window> window,
    std::shared_ptr<SpriteSheet> spriteSheet,
    std::shared_ptr<SpriteBuffer> spriteBuffer,
    std::shared_ptr<ShaderProgram> shaderProgram
) : 
_window(window), 
_spriteSheet(spriteSheet), 
_spriteBuffer(spriteBuffer),
_shaderProgram(shaderProgram)
{}

void SpriteRenderer::init() {
    _view = glm::mat4(1.0);
    _fov = 45.f;
    const float vertices[] = {
        // positions
         1.0f, 0.0f, // top right
         1.0f, 1.0f, // bottom right
         0.0f, 0.0f, // top left 

         1.0f, 1.0f, // bottom right
         0.0f, 1.0f, // bottom left
         0.0f, 0.0f // top left 
    };
    auto sheetSize = _spriteSheet->getSize();


    _texture = _shaderProgram->loadTexture(_spriteSheet->getRawImage(), sheetSize.x, sheetSize.y);
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->linkShaders();

    _shaderProgram->loadData(vertices);
    _shaderProgram->defineAttribute<float>("aPos", 2);
    _shaderProgram->bindAttributes();

    _vbo1 = _shaderProgram->initInstanceBuffer();
    _shaderProgram->defineInstanceAttribute<glm::vec4>(_vbo1, "instanceModel", 4);
    _shaderProgram->bindAttributes(_vbo1);

    _vbo2 = _shaderProgram->initInstanceBuffer();
    _shaderProgram->defineInstanceAttribute<glm::vec4>(_vbo2, "instanceTexModel", 4);
    _shaderProgram->bindAttributes(_vbo2);
}

void SpriteRenderer::setView(glm::mat4 view) 
{
    _view = view;
}

void SpriteRenderer::setProjection(glm::mat4 projection) 
{
    _projection = projection;
}

void SpriteRenderer::draw() {
    if (_spriteBuffer->size() == 0) {
        return;
    }
    _shaderProgram->use();
    _shaderProgram->bindTexture(_texture);
    if (_spriteBuffer->areTexturesDirty()) {
        _shaderProgram->loadInstanceData(_vbo2, sizeof(glm::mat4) * _spriteBuffer->size(), _spriteBuffer->size(), _spriteBuffer->textureData());
    }
    _shaderProgram->loadInstanceData(_vbo1, sizeof(glm::mat4) *  _spriteBuffer->size(), _spriteBuffer->size(), _spriteBuffer->modelData());
    _shaderProgram->setUniform<int>("spriteSheet", _texture);
    _shaderProgram->setUniform("projection", _projection);
    _shaderProgram->setUniform("view", _view);
    _shaderProgram->drawInstances();
    _shaderProgram->unbindTextures();
    _shaderProgram->unbindVao();
    _spriteBuffer->resetDirtyFlag();
    _spriteBuffer->step();
}