#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include "texture.h"

#include <cstddef>
#include <gl/glew.h>
#include <vector>
#include <string>

class ShaderProgram {
public:
    void loadTexture(const char* filePath, const char* textureName);

    template<typename>
    void defineAttribute(GLuint position, GLint dimensions);

    void addVertexShader(const char* data);
    void addFragmentShader(const char* data);
    void linkShaders();
    void bindAttributes();

    template<std::size_t S, typename T>
    void loadData(T (&data)[S]);

    template<std::size_t S>
    void loadIndices(const GLuint (&data)[S]);

    void bindVao();
    void unbindVao();

    void bindTextures();
    void initTextures();

    template<typename T>
    void setUniform(const std::string &name, T value);

    void use();
    void drawArrays();
    void drawElements();

    void init();

    void clearScreen(float r, float g, float b, float a);
    ~ShaderProgram();
private:
    template<typename>
    struct GlTypes;

    struct Attribute {
        GLuint      position;
        GLint       dimensions;
        GLenum      type;
        std::size_t size;
    };

    bool                     _programCreated = false;
    bool                     _vaoInitialized = false;
    GLuint                   _programId;
    GLuint                   _vao;
    GLuint                   _vbo;
    GLuint                   _ebo;
    std::vector<GLuint>      _textures;
    std::vector<const char*> _textureNames;
    std::vector<GLuint>      _shaders;
    std::vector<Attribute>   _attributes;
    GLsizei                  _numIndices;

    

    void   _addShader(GLenum shaderType, const char* data);
    GLuint _getOrCreateProgramId();
    void _defineAttribute(GLuint position, GLint dimensions, GLenum type, std::size_t size);
    void _loadData(GLsizeiptr size, const GLvoid* data);
    void _loadIndices(std::size_t size, const GLuint* data);
};

template<>
struct ShaderProgram::GlTypes<char> {
    static constexpr GLenum type = GL_BYTE;
};

template<>
struct ShaderProgram::GlTypes<unsigned char> {
    static constexpr GLenum type = GL_UNSIGNED_BYTE;
};

template<>
struct ShaderProgram::GlTypes<short> {
    static constexpr GLenum type = GL_SHORT;
};
template<>
struct ShaderProgram::GlTypes<int> {
    static constexpr GLenum type = GL_INT;
};

template<>
struct ShaderProgram::GlTypes<unsigned int> {
    static constexpr GLenum type = GL_UNSIGNED_INT;
};

template<>
struct ShaderProgram::GlTypes<float> {
    static constexpr GLenum type = GL_FLOAT;
};

template<>
struct ShaderProgram::GlTypes<double> {
    static constexpr GLenum type = GL_DOUBLE;
};

template <typename T>
inline void ShaderProgram::defineAttribute(GLuint position, GLint dimensions) 
{
    GLenum type = ShaderProgram::GlTypes<T>::type;
    _defineAttribute(position, dimensions, type, sizeof(T));
}

template<std::size_t S>
inline void ShaderProgram::loadIndices(const GLuint (&data)[S])
{
    _loadIndices(S * sizeof(GLuint), data);
}

template<std::size_t size, typename T>
inline void ShaderProgram::loadData(T (&data)[size]) 
{
    _loadData(size * sizeof(T), data);
}


template<>
inline void ShaderProgram::setUniform(const std::string& name, bool value)
{
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), static_cast<int>(value));
}

template<>
inline void ShaderProgram::setUniform(const std::string& name, int value)
{
    glUniform1i(glGetUniformLocation(_programId, name.c_str()), value);
}

template<>
inline void ShaderProgram::setUniform(const std::string& name, float value)
{
    glUniform1f(glGetUniformLocation(_programId, name.c_str()), value);
}

template<>
inline void ShaderProgram::setUniform(const std::string& name, double value)
{
    glUniform1d(glGetUniformLocation(_programId, name.c_str()), value);
}

#endif
