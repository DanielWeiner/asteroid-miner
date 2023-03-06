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
static constexpr int MODEL_SIZE = ((sizeof(glm::mat4) + sizeof(float)) / sizeof(float));

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
layout (location = 0) in mat4 inModel;
layout (location = 4) in float inSpriteIndex;

out mat4 model;
out float spriteIndex;

void main()
{
    model = inModel;
    spriteIndex = inSpriteIndex;
}  
/*--------------------------------------------*/
)glsl";

const char* geometryShaderSource = R"glsl(
/*------------------GEOMETRY------------------*/
#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = MAX_VERTICES) out;
layout(std430, binding = 0) readonly buffer spriteGeometry
{ 
    int[SPRITE_COUNT]   trianglesOffset;
    int[SPRITE_COUNT]   trianglesCount;

    int[SPRITE_COUNT]   verticesOffset;
    int[SPRITE_COUNT]   verticesCount;

    int[NUM_TRIANGLES]  triangles;
    float[NUM_VERTICES] vertices;
};

out vec2 TexCoord;

in mat4[] model;
in float[] spriteIndex;

uniform mat4 projection;
uniform mat4 view;

void main() {
    int idx = int(spriteIndex[0]);

    int trianglesStart = trianglesOffset[idx];
    int verticesStart = verticesOffset[idx];
    
    for (int i = 0; i < trianglesCount[idx]; i++) {
        int triangle = trianglesStart + i * 3;

        for (int j = 0; j < 3; j++) {
            int vertex = verticesStart + triangles[triangle + j] * 4;

            float x = vertices[vertex + 0];
            float y = vertices[vertex + 1];
            float uvX = vertices[vertex + 2];
            float uvY = vertices[vertex + 3];

            gl_Position = projection * view * model[0] * vec4(x, y, 0.0, 1.0);
            TexCoord = vec2(uvX, uvY);
            EmitVertex();
        }

        EndPrimitive();
    }
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
    std::shared_ptr<Window> window,
    std::shared_ptr<SpriteSheet> spriteSheet,
    std::shared_ptr<SpriteBuffer> spriteBuffer,
    std::shared_ptr<ShaderProgram> shaderProgram,
    bool useLinearScaling
) : 
_window(window), 
_spriteSheet(spriteSheet), 
_spriteBuffer(spriteBuffer),
_shaderProgram(shaderProgram),
_useLinearScaling(useLinearScaling)
{}

void SpriteRenderer::init() {
    _view = glm::mat4(1.0);
    _fov = 45.f;
    auto sheetSize = _spriteSheet->getSize();

    std::string geometryShaderStr(geometryShaderSource);
    geometryShaderStr = replace(geometryShaderStr, "MAX_VERTICES", _spriteSheet->getMaxVertices());
    geometryShaderStr = replace(geometryShaderStr, "SPRITE_COUNT", _spriteSheet->getSpriteCount());
    geometryShaderStr = replace(geometryShaderStr, "NUM_VERTICES", _spriteSheet->getVertexScalarCount());
    geometryShaderStr = replace(geometryShaderStr, "NUM_TRIANGLES", _spriteSheet->getTriangleScalarCount());

    _texture = _shaderProgram->loadTexture(_spriteSheet->getRawImage(), sheetSize.x, sheetSize.y, 4, _useLinearScaling);
    _shaderProgram->addVertexShader(vertexShaderSource);
    _shaderProgram->addGeometryShader(geometryShaderStr.c_str());
    _shaderProgram->addFragmentShader(fragmentShaderSource);
    _shaderProgram->linkShaders();

    _shaderProgram->initShaderStorageBuffer();
    _shaderProgram->loadShaderStorageData(
        0, 
        _spriteSheet->getBufferSize(), 
        _spriteSheet->getSpriteInfoBuffer()
    );

    _shaderProgram->initVertexBuffer();
    _shaderProgram->defineAttribute<glm::vec4>("inModel", 4);
    _shaderProgram->defineAttribute<float>("inSpriteIndex", 1);
    _shaderProgram->bindAttributes();
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
    _shaderProgram->loadData(_spriteBuffer->size() * sizeof(float), _spriteBuffer->size() / MODEL_SIZE, _spriteBuffer->modelData());
    _shaderProgram->setUniform<int>("spriteSheet", _texture);
    _shaderProgram->setUniform("projection", _projection);
    _shaderProgram->setUniform("view", _view);
    _shaderProgram->drawArrays(GL_POINTS);
    _shaderProgram->unbindTextures();
    _shaderProgram->unbindVao();
    _spriteBuffer->resetDirtyFlag();
    _spriteBuffer->step();
}