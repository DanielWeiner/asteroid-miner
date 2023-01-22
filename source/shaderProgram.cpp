#include "shaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

void ShaderProgram::init() 
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
}

void ShaderProgram::clearScreen(float r, float g, float b, float a) 
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

ShaderProgram::~ShaderProgram() 
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
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
    int success;
    char infoLog[512];
    glLinkProgram(_getOrCreateProgramId());
    // check for linking errors
    glGetProgramiv(_getOrCreateProgramId(), GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_getOrCreateProgramId(), 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    for (auto it = _shaders.begin(); it < _shaders.end(); it++) {
        glDeleteShader(*it);
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
    glUseProgram(_programId);
}

void ShaderProgram::drawArrays() 
{
    glUseProgram(_programId);
    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void ShaderProgram::drawElements() 
{
    glUseProgram(_programId);
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
}

void ShaderProgram::bindAttributes() {
    GLsizei stride = 0;
    for (auto it = _attributes.begin(); it < _attributes.end(); it++) {
        stride += it->dimensions * it->size;
    }

    long long offset = 0;
    for (auto it = _attributes.begin(); it < _attributes.end(); it++) {
        glVertexAttribPointer(
            it->position,
            it->dimensions,
            it->type,
            GL_FALSE,
            stride,
            (void*)offset
        );
        offset += it->dimensions * it->size;
        glEnableVertexAttribArray(it->position);
    }
    
    _attributes.clear();
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void ShaderProgram::_defineAttribute(GLuint position, GLint dimensions, GLenum type, std::size_t size)
{
    _attributes.push_back({
        .position   = position,
        .dimensions = dimensions,
        .type       = type,
        .size       = size
    });
}

void ShaderProgram::bindVao() 
{
    glBindVertexArray(_vao);
}

void ShaderProgram::unbindVao() 
{
    glBindVertexArray(0);
}

void ShaderProgram::initTextures() 
{
    int i = 0;
    for (auto it = _textures.begin(); it < _textures.end(); it++) {
        glActiveTexture(GL_TEXTURE0 + i++);
        glBindTexture(GL_TEXTURE_2D, *it);
    }
}
void ShaderProgram::bindTextures() 
{
    int i = 0;
    for (auto it = _textures.begin(); it < _textures.end(); it++) {
        glUniform1i(glGetUniformLocation(_programId, _textureNames[i]), i);
        i++;
    }
}

GLuint ShaderProgram::_getOrCreateProgramId() {
    if (!_programCreated) {
        _programId = glCreateProgram();
        _programCreated = true;
    }
    return _programId;
}

void ShaderProgram::_loadData(GLsizeiptr size, const GLvoid* data)
{
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void ShaderProgram::_loadIndices(std::size_t size, const GLuint* data)
{
    _numIndices = size;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
