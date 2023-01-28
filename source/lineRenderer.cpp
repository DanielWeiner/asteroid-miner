#include "lineRenderer.h"
#include "shaderProgram.h"

namespace {

const char* vertexShaderSource = R"glsl("
/*-------------------VERTEX------------------*/
#version 330 core
layout (location = 0) in vec2 inPos;
layout (location = 1) in vec4 inColor;

out vec2 TexCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * instanceModel * vec4(aPos.xy, 0.0, 1.0);
    TexCoord = vec2(instanceTexModel * vec4(aPos.xy, 0.0, 1.0));
}  
/*--------------------------------------------*/
")glsl";

const char* fragmentShaderSource = R"glsl("
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
")glsl";


}

void LineRenderer::init() 
{
    _shaderProgram = std::make_unique<ShaderProgram>();
    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->linkShaders();

    _shaderProgram->initVertexBuffer();
}

void LineRenderer::lineTo(glm::vec2 from, glm::vec2 to) 
{

}

void LineRenderer::draw()
{

}
