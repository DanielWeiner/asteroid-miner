#include "shaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <algorithm>
#include <string>

namespace {
    typedef void (*InitFn)(GLsizei, GLuint*);

    void initVar(InitFn fn, std::unique_ptr<GLuint> &ptr) {
        if (ptr == nullptr) {
            ptr.reset(new GLuint());
            fn(1, ptr.get());
        }
    }

    void GLAPIENTRY
    MessageCallback( GLenum source,
                    GLenum type,
                    GLuint id,
                    GLenum severity,
                    GLsizei length,
                    const GLchar* message,
                    const void* userParam )
    {
        std::string msg(message);
        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                    type, severity, msg.c_str() );
    }

    // During init, enable debug output
    
}

void ShaderProgram::init()
{
    glEnable              (GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
}

void ShaderProgram::clearScreen(float r, float g, float b, float a) 
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

ShaderProgram::~ShaderProgram() 
{
    if (_vao != nullptr) {
        glDeleteVertexArrays(1, _vao.get());
    }
    if (_vbo != nullptr) {
        glDeleteBuffers(1, _vbo.get());
    }

    if (_ebo != nullptr) {
        glDeleteBuffers(1, _ebo.get());
    }

    if (_instanceVbo != nullptr) {
        glDeleteBuffers(1, _instanceVbo.get());
    }

    for (auto texture : _textures) {
        glDeleteTextures(1, &texture);
    }

    if (_programId != nullptr) {
        glDeleteProgram(*_programId);
    }
}

void ShaderProgram::loadTexture(const char* filepath, const char* textureName) {
    GLuint texture;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    _textures.push_back(texture);
    _textureNames.push_back(textureName);
}

void ShaderProgram::addVertexShader(const char *data) 
{
    _addShader(GL_VERTEX_SHADER, data);
}

void ShaderProgram::addFragmentShader(const char *data) 
{
    _addShader(GL_FRAGMENT_SHADER, data);
}

void ShaderProgram::linkShaders() 
{
    GLuint programId = _getOrCreateProgramId();
    int success;
    char infoLog[512];
    glLinkProgram(programId);
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    for (auto shader : _shaders) {
        glDeleteShader(shader);
    }
    _shaders.clear();
}

void ShaderProgram::_addShader(GLenum shaderType, const char* data) 
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &data, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glAttachShader(_getOrCreateProgramId(), shader);
    _shaders.push_back(shader);
}

void ShaderProgram::use() 
{
    glUseProgram(*_programId);
}

void ShaderProgram::drawArrays() 
{
    glUseProgram(*_programId);
    glBindVertexArray(*_vao);
    glDrawArrays(GL_TRIANGLES, 0, _numVertices);
}

void ShaderProgram::drawInstances() 
{
    glUseProgram(*_programId);
    glBindVertexArray(*_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, _numVertices, _numInstances);
}

void ShaderProgram::drawElements() 
{
    glUseProgram(*_programId);
    glBindVertexArray(*_vao);
    glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
}

void ShaderProgram::bindAttributes() {
    static const auto vec4size = sizeof(glm::vec4);

    GLsizei stride = 0;
    for (auto attribute : _attributes) {
        stride += attribute.dimensions * attribute.size;
    }

    long long offset = 0;
    GLuint boundBuffer = 0;
    for (auto attribute : _attributes) {
        auto instance = attribute.instance;
        auto dimension = attribute.dimensions;
        auto size = attribute.size;
        auto type = attribute.type;
        auto position = attribute.position;
        
        if (instance && boundBuffer != *_instanceVbo) {
            boundBuffer = *_instanceVbo;
            glBindBuffer(GL_ARRAY_BUFFER, *_instanceVbo);
        } else if (!instance && boundBuffer != *_vbo) {
            boundBuffer = *_vbo;
            glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
        }

        GLuint i = 0;
        for (auto chunkSize = dimension * size; chunkSize > 0; chunkSize -= vec4size) {
            int realSize = std::min(chunkSize, (int)vec4size);
            int dim = realSize / (vec4size / 4);
            glEnableVertexAttribArray(position + i);
            glVertexAttribPointer(
                position + i,
                dim,
                type,
                GL_FALSE,
                stride,
                (void*)offset
            );
            offset += realSize;
            
            if (instance) {
                glVertexAttribDivisor(position + i, 1);
            }
            i++;
        }
    }
    
    _attributes.clear();
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void ShaderProgram::_defineAttribute(
    const char* name, 
    GLint dimensions, 
    GLenum type, 
    GLsizei size,
    bool instance
)
{
    GLuint position = glGetAttribLocation(*_programId, name);
    _attributes.push_back({
        .position   = position,
        .dimensions = dimensions,
        .type       = type,
        .size       = size,
        .instance   = instance
    });
}

void ShaderProgram::bindVao() 
{
    glBindVertexArray(*_vao);
}

void ShaderProgram::unbindVao() 
{
    glBindVertexArray(0);
}

void ShaderProgram::initTextures() 
{
    int i = 0;
    for (auto texture : _textures) {
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}
void ShaderProgram::bindTextures() 
{
    use();
    int i = 0;
    for (auto texture : _textures) {
        glUniform1i(glGetUniformLocation(*_programId, _textureNames[i]), texture);
        i++;
    }
}

GLuint ShaderProgram::_getOrCreateProgramId() {
    if (_programId == nullptr) {
        _programId.reset(new GLuint(glCreateProgram()));
    }
    return *_programId;
}

void ShaderProgram::loadData(GLsizeiptr size, GLsizei count, const GLvoid* data)
{
    _initVao();
    _initVbo();
    bindVao();
    _numVertices = count;
    glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void ShaderProgram::loadIndices(GLsizei size, GLsizei count, const GLuint* data)
{
    _initVao();
    _initEbo();
    bindVao();
    _numIndices = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void ShaderProgram::_initVao() 
{
    initVar(glGenVertexArrays, _vao);
}

void ShaderProgram::_initVbo() 
{
    initVar(glGenBuffers, _vbo);
}

void ShaderProgram::initInstanceBuffer() 
{
    _initVao();
    _initInstanceVbo();
    bindVao();
    glBindBuffer(GL_ARRAY_BUFFER, *_instanceVbo);
}

void ShaderProgram::loadInstanceData(GLsizeiptr size, GLsizei count, const GLvoid *data) 
{
    _initVao();
    _initInstanceVbo();
    bindVao();
    _numInstances = count;
    glBindBuffer(GL_ARRAY_BUFFER, *_instanceVbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void ShaderProgram::_initInstanceVbo() 
{
    initVar(glGenBuffers, _instanceVbo);
}

void ShaderProgram::_initEbo() 
{
    initVar(glGenBuffers, _ebo);
}

GLuint ShaderProgram::_getUniformLocation(const char *name) 
{ 
    return glGetUniformLocation(*_programId, name);
}
