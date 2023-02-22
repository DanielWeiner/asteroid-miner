#include "lineRenderer.h"

#include "shaderProgram.h"

#include <cmath>
#include <glm/ext.hpp>
#include <glad/glad.h>
namespace {

const char* vertexShaderSource = R"glsl(
/*-------------------VERTEX------------------*/
#version 330 core

layout (location = 0) in vec2 inPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = projection * view * modelMatrix * vec4(inPos.xy, 0.0, 1.0);
}  
/*--------------------------------------------*/
)glsl";

const char* fragmentShaderSource = R"glsl(
/*------------------FRAGMENT------------------*/
#version 330 core

out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = color;
}
/*--------------------------------------------*/
)glsl";


}

LineRenderer::LineRenderer(std::shared_ptr<Window> window) : _window(window) {}

void LineRenderer::init() {
    _shaderProgram = std::make_unique<ShaderProgram>();
    _shaderProgram->init();
    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->linkShaders();

    _shaderProgram->initVertexBuffer();
    _shaderProgram->defineAttribute<float>("inPos", 2);
    _shaderProgram->bindAttributes();

    const float vertices[]{ 
        0.f, 0.f,
        1.f, 0.f,
        0.f, 1.f,

        1.f, 0.f,
        0.f, 1.f,
        1.f, 1.f
    };
    _shaderProgram->loadData(vertices);
}

void LineRenderer::setProjection(glm::mat4 projection) 
{
    _projection = projection;
}

void LineRenderer::setView(glm::mat4 view) 
{
    _view = view;
}

void LineRenderer::lineTo(glm::vec2 from, glm::vec2 to, glm::vec4 color, float width) 
{
    auto angleVector = to - from;
    auto distance = sqrt(angleVector.x * angleVector.x + (angleVector.y) * (angleVector.y));

    glm::mat4 modelMatrix(1.0);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(from.x, from.y, 0.0));
    modelMatrix = glm::rotate(modelMatrix, atan2(angleVector.y, angleVector.x), glm::vec3(0.0, 0.0, 1.0));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -width / 2, 0.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(distance, width, 1.0f));

    _shaderProgram->use();
    _shaderProgram->setUniform("projection", _projection);
    _shaderProgram->setUniform("view", _view);
    _shaderProgram->setUniform("modelMatrix", modelMatrix);
    _shaderProgram->setUniform("color", color);
    _shaderProgram->drawArrays();
}