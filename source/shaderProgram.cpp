#include "shaderProgram.h"

#include <iostream>
#include <algorithm>
#include <string>

namespace {
    typedef void (*InitFn)(GLsizei, GLuint*);

    void initVar(InitFn fn, GLuint* ptr) {
        fn(1, ptr);
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
    static bool initialized = false;
    if (!initialized) {
        glEnable              (GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
        
        initialized = true;
    }
}

ShaderProgram::~ShaderProgram() 
{
    if (_vao) {
        glDeleteVertexArrays(1, _vao.get());
    }

    if (_vbo) {
        glDeleteBuffers(1, _vbo.get());
    }

    if (_ebo) {
        glDeleteBuffers(1, _ebo.get());
    }

    for(auto vbo : _instanceVbos) {
        glDeleteBuffers(1, &vbo);
    }

    for (auto texture : _textures) {
        glDeleteTextures(1, &texture);
    }

    if (_programId) {
        glDeleteProgram(*_programId);
    }
}

GLuint ShaderProgram::loadTexture(unsigned char* image, int width, int height, bool useLinearScaling) {
    GLuint texture;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    if (useLinearScaling) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    _textures.push_back(texture);

    return texture;
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

void ShaderProgram::drawArrays(GLenum arrayType) 
{
    glBindVertexArray(*_vao);
    glDrawArrays(arrayType, 0, _numVertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbindVao();
}

void ShaderProgram::drawInstances() 
{
    glBindVertexArray(*_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, _numVertices, _numInstances);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbindVao();
}

void ShaderProgram::drawElements() 
{
    glBindVertexArray(*_vao);
    glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    unbindVao();
}

void ShaderProgram::bindAttributes() {
    _bindAttributes(*_vbo);
}

void ShaderProgram::bindAttributes(unsigned int id) {
    _bindAttributes(_instanceVbos[id]);
}

void ShaderProgram::_bindAttributes(GLuint id) {
    static const auto vec4size = sizeof(glm::vec4);

    GLsizei stride = 0;
    for (auto attribute : _attributes) {
        if (attribute.vbo == id) {
            stride += attribute.dimensions * attribute.size;
        }
    }

    long long offset = 0;
    GLuint boundBuffer = 0;
    for (auto attribute : _attributes) {
        if (boundBuffer != attribute.vbo) {
            if (id == attribute.vbo) {
                glBindBuffer(GL_ARRAY_BUFFER, id);
            }
            else {
                continue;
            }
        }

        auto instance = attribute.instance;
        auto dimension = attribute.dimensions;
        auto size = attribute.size;
        auto type = attribute.type;
        auto position = attribute.position;
        
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
    bool instance,
    GLuint vbo
)
{
    GLuint position = glGetAttribLocation(*_programId, name);
    _attributes.push_back({
        .position   = position,
        .dimensions = dimensions,
        .type       = type,
        .size       = size,
        .instance   = instance,
        .vbo        = vbo
    });
}

void ShaderProgram::bindVao() 
{
    glBindVertexArray(*_vao);
}

void ShaderProgram::unbindVao() 
{
    glBindVertexArray(0);
    glUseProgram(0);
}

void ShaderProgram::bindTexture(GLuint texture) 
{
    glActiveTexture(GL_TEXTURE0 + texture);
    glBindTexture(GL_TEXTURE_2D, texture);
}
void ShaderProgram::unbindTextures() 
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
}

GLuint ShaderProgram::_getOrCreateProgramId() {
    if (_programId == nullptr) {
        _programId.reset(new GLuint(glCreateProgram()));
    }
    return *_programId;
}

void ShaderProgram::loadData(GLsizei size, GLsizei count, const GLvoid* data)
{
    _initVao();
    _initVbo();
    bindVao();
    _numVertices = count;
    glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, _vboType);
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
    if (!_vao) {
        GLuint vao;
        initVar(glGenVertexArrays, &vao);
        _vao = std::make_unique<GLuint>(vao);
    }
}

void ShaderProgram::_initVbo() 
{
    if (!_vbo) {
        GLuint vbo;
        initVar(glGenBuffers, &vbo);
        _vbo = std::make_unique<GLuint>(vbo);
    }
}

void ShaderProgram::_initInstanceVbo() 
{
    GLuint instanceVbo;
    initVar(glGenBuffers, &instanceVbo);
    _instanceVbos.push_back(instanceVbo);
}

const unsigned int ShaderProgram::initInstanceBuffer(GLenum bufType) 
{
    auto id = _instanceVbos.size();
    _instanceVboTypes.push_back(bufType);

    _initVao();
    _initInstanceVbo();
    bindVao();
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVbos[id]);

    return id;
}

void ShaderProgram::initVertexBuffer(GLenum type)
{
    _vboType = type;
    _initVao();
    _initVbo();
    bindVao();
    glBindBuffer(GL_ARRAY_BUFFER, *_vbo);
}

void ShaderProgram::loadInstanceData(unsigned int id, GLsizeiptr size, GLsizei count, const GLvoid *data) 
{
    _initVao();
    bindVao();
    _numInstances = count;
    glBindBuffer(GL_ARRAY_BUFFER, _instanceVbos[id]);
    glBufferData(GL_ARRAY_BUFFER, size, data, _instanceVboTypes[id]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ShaderProgram::_initEbo() 
{
    if (!_ebo) {
        GLuint ebo;
        initVar(glGenBuffers, &ebo);
        _ebo = std::make_unique<GLuint>(ebo);
    }
}

GLuint ShaderProgram::_getUniformLocation(const char *name) 
{ 
    return glGetUniformLocation(*_programId, name);
}
