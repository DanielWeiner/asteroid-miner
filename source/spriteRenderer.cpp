#include "spriteRenderer.h"

#include "spriteSheet.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <random>

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

void SpriteRenderer::draw() 
{
    static const int numSprites = 1000;
    static bool initialized = false;
    static glm::mat4 spriteData[2 * numSprites];

    if (!initialized) {
        initialized = true;
        std::string sprites[] = {
            "playerShip1_blue.png",
            "playerShip1_green.png",
            "playerShip1_orange.png",
            "playerShip1_red.png",
            "playerShip2_blue.png",
            "playerShip2_green.png",
            "playerShip2_orange.png",
            "playerShip2_red.png",
            "playerShip3_blue.png",
            "playerShip3_green.png",
            "playerShip3_orange.png",
            "playerShip3_red.png"
        };

        for (int i = 0; i < numSprites; i++) {
            auto sprite = sprites[rand() % (sizeof(sprites)/sizeof(std::string))];
            auto x = (float)(rand() % 2560);
            auto y = (float)(rand() % 1334);
            auto rotate = (float)(rand() % 360);

            auto texMatrix = spriteSheet.getSpriteMatrix(sprite.c_str());
            auto dimensions = spriteSheet.getRawDimensions(sprite.c_str());
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x - dimensions.x / 2, y - dimensions.y / 2, 0.0f));

            model = glm::translate(model, glm::vec3(0.5f * dimensions.x, 0.5f * dimensions.y, 0.0f)); 
            model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); 
            model = glm::translate(model, glm::vec3(-0.5f * dimensions.x, -0.5f * dimensions.y, 0.0f));

            model = glm::scale(model, glm::vec3(dimensions, 1.0f));
            
            spriteData[2 * i] = model;
            spriteData[2 * i + 1] = texMatrix;
        }
    }

    glm::mat4 projection = glm::ortho(0.0f, (float)_window->getDimensions().x, 
        (float)_window->getDimensions().y, 0.0f, -1.0f, 1.0f);

    _shaderProgram->use();
    _shaderProgram->loadInstanceData(sizeof(spriteData), numSprites, &spriteData[0]);
    _shaderProgram->setUniform("projection", projection);
    _shaderProgram->bindVao();
    _shaderProgram->drawInstances();
}
