#include "spriteRenderer.h"

#include "sprite.h"
#include "spriteSheet.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <exception>

namespace  {

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in mat4 instanceModel;
layout (location = 5) in mat4 instanceTexModel;

out vec2 TexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * instanceModel * vec4(aPos.xy, 0.0, 1.0);
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

namespace {
    SpriteSheet spriteSheet("data/sprites/sprites.json");
}

SpriteRenderer::SpriteRenderer() : SpriteRenderer(0.f, 0.f) {}

SpriteRenderer::SpriteRenderer(
    float width,
    float height
) 
: _shaderProgram(ShaderProgram()), _width(width), _height(height)
{}

void SpriteRenderer::init() {
    spriteSheet.load();

    const float vertices[] = {
        // positions
         1.0f, 0.0f, // top right
         1.0f, 1.0f, // bottom right
         0.0f, 0.0f, // top left 

         1.0f, 1.0f, // bottom right
         0.0f, 1.0f, // bottom left
         0.0f, 0.0f // top left 
    };

    _shaderProgram.init();
    _shaderProgram.loadTexture("data/sprites/sprites.png", "spriteSheet");
    _shaderProgram.addFragmentShader(fragmentShaderSource);
    _shaderProgram.addVertexShader(vertexShaderSource);
    _shaderProgram.linkShaders();

    _shaderProgram.loadData(vertices);
    _shaderProgram.defineAttribute<float>("aPos", 2);
    _shaderProgram.bindAttributes();

    _vbo1 = _shaderProgram.initInstanceBuffer(GL_STREAM_DRAW);
    _shaderProgram.defineInstanceAttribute<glm::vec4>(_vbo1, "instanceModel", 4);
    _shaderProgram.bindAttributes(_vbo1);

    _vbo2 = _shaderProgram.initInstanceBuffer();
    _shaderProgram.defineInstanceAttribute<glm::vec4>(_vbo2, "instanceTexModel", 4);
    _shaderProgram.bindAttributes(_vbo2);

    _shaderProgram.initTextures();
}

void SpriteRenderer::setBuffer(const unsigned int size) 
{
    _bufSize = size;
    _length = 0;
    _buffer = std::unique_ptr<glm::mat4[]>(new glm::mat4[size]);
    _texBuffer = std::unique_ptr<glm::mat4[]>(new glm::mat4[size]);
}

void SpriteRenderer::updateDimensions(float width, float height) 
{
    _width = width;
    _height = height;
}

std::unique_ptr<Sprite> SpriteRenderer::createSprite(const char* name)
{
    if (_length >= _bufSize) {
        throw std::exception("Sprite buffer exceeded");
    }
    _spritesDirty = true;
    glm::mat4* buf = &_buffer[_length];
    
    auto sprite = std::make_unique<Sprite>(Sprite(name, buf));
    spriteSheet.writeSpriteMatrix(name, &_texBuffer[ _length]);
    
    ++_length;
    
    sprite->scaleBy(spriteSheet.getRawDimensions(name));
    return std::move(sprite);
}

void SpriteRenderer::draw() 
{
    glm::mat4 projection = glm::ortho(0.0f, _width, _height, 0.0f, -1.0f, 1.0f);

    _shaderProgram.use();
    if (_spritesDirty) {
        _shaderProgram.loadInstanceData(_vbo2, sizeof(glm::mat4) * _length, _length, _texBuffer.get());
        _spritesDirty = false;
    }
    _shaderProgram.loadInstanceData(_vbo1, sizeof(glm::mat4) * _length, _length, _buffer.get());
    _shaderProgram.setUniform("projection", projection);
    _shaderProgram.bindVao();
    _shaderProgram.drawInstances();
}

void SpriteRenderer::clearScreen() 
{
    _shaderProgram.clearScreen();
}
