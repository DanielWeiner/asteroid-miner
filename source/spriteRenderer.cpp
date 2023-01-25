#include "spriteRenderer.h"

#include "sprite.h"
#include "spriteSheet.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <string>

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

SpriteRenderer::SpriteRenderer(
    ShaderProgram* shaderProgram,
    Window* window
) 
: _shaderProgram(shaderProgram), _window(window)
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

    _shaderProgram->init();
    _shaderProgram->loadTexture("data/sprites/sprites.png", "spriteSheet");
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->linkShaders();

    _shaderProgram->loadData(vertices);
    _shaderProgram->defineAttribute<float>("aPos", 2);
    _shaderProgram->bindAttributes();

    _shaderProgram->initInstanceBuffer();
    _shaderProgram->defineInstanceAttribute<glm::vec4>("instanceModel", 4);
    _shaderProgram->defineInstanceAttribute<glm::vec4>("instanceTexModel", 4);
    _shaderProgram->bindAttributes();
    _shaderProgram->initTextures();
}

std::shared_ptr<Sprite> SpriteRenderer::createSprite(const char* name)
{
    auto sprite = std::shared_ptr<Sprite>(new Sprite(name));
    sprite->scaleBy(*spriteSheet.getRawDimensions(name));
    return sprite;
}

void SpriteRenderer::addSprite(std::shared_ptr<Sprite> sprite) 
{
    _sprites.push_back(sprite);
}

void SpriteRenderer::draw() 
{
    if (_sprites.size() == 0) {
        return;
    }

    glm::mat4* buffer = new glm::mat4[_sprites.size() * 2];

    int spriteCount = 0;
    for (auto& sprite: _sprites) {
        buffer[spriteCount * 2] = *sprite->getModelMatrix(),
        buffer[spriteCount * 2 + 1] = *spriteSheet.getSpriteMatrix(sprite->getName());
        spriteCount++;
    }

    glm::mat4 projection = glm::ortho(0.0f, (float)_window->getDimensions().x, 
        (float)_window->getDimensions().y, 0.0f, -1.0f, 1.0f);

    _shaderProgram->use();
    _shaderProgram->loadInstanceData(sizeof(glm::mat4) * 2 * spriteCount, spriteCount, buffer);
    _shaderProgram->setUniform("projection", projection);
    _shaderProgram->bindVao();
    _shaderProgram->drawInstances();
    delete[] buffer;
    _sprites.clear();
}