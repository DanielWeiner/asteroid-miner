#include "spriteRenderer.h"

#include "spriteSheet.h"
#include <memory>
#include <iostream>
#include <string>

namespace 
{
    const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}  
/*--------------------------------------------*/
)glsl";

    const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
/*--------------------------------------------*/
)glsl";
}

SpriteRenderer::SpriteRenderer(const ShaderProgram& shaderProgram) 
: _shaderProgram(shaderProgram)
{}
void SpriteRenderer::init() 
{
    SpriteSheet spriteSheet("data/sprites/sprites.json");
    spriteSheet.load();
    auto boundingBox = spriteSheet.getSpriteBoundingBox("playerShip1_blue.png");
    //auto boundingBox = spriteSheet.getSpriteBoundingBox("playerShip1_red.png");
    auto x = boundingBox.x;
    auto y = boundingBox.y;
    auto width = boundingBox.width;
    auto height = boundingBox.height;

    const float vertices[] = {
         // positions         // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   x + width, y,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   x + width, y - height,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   x, y - height,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   x, y    // top left 
    };

    const unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    

    _shaderProgram.loadTexture(
        "data/sprites/sprites.png",
        "ourTexture"
    );
    _shaderProgram.addFragmentShader(fragmentShaderSource);
    _shaderProgram.addVertexShader(vertexShaderSource);
    _shaderProgram.linkShaders();
    _shaderProgram.init();
    
    _shaderProgram.bindVao();
    _shaderProgram.loadData(vertices);
    _shaderProgram.loadIndices(indices);
    _shaderProgram.defineAttribute<float>(0, 3);
    _shaderProgram.defineAttribute<float>(1, 3);
    _shaderProgram.defineAttribute<float>(2, 2);
    _shaderProgram.bindAttributes();
    _shaderProgram.initTextures();
}

void SpriteRenderer::draw() 
{
    _shaderProgram.use();
    _shaderProgram.bindVao();
    _shaderProgram.drawElements();
}
