#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <glad/glad.h>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <string>
#include <memory>

class ShaderProgram {
public:
    GLuint loadTexture(unsigned char* image, int width, int height, int colorChannels, bool useLinearScaling);

    template<typename>
    void defineAttribute(const char* name, GLint dimensions);

    template<typename>
    void defineInstanceAttribute(unsigned int id, const char* name, GLint dimensions);

    void addVertexShader(const char* data);
    void addFragmentShader(const char* data);
    void addGeometryShader(const char* data);
    void linkShaders();
    void bindAttributes();
    void bindAttributes(unsigned int id);

    template<GLsizei S, typename T>
    void loadData(T (&data)[S]);
    template<GLsizei S, typename T>
    void loadInstanceData(unsigned int id, T (&data)[S]);
    template<GLsizei S>
    void loadIndices(const GLuint (&data)[S]);
    template<GLsizei S, typename T>
    void loadShaderStorageData(int bindIndex, T (&data)[S]);

    /// @brief 
    /// @param size  total buffer size
    /// @param count vertex object count
    /// @param data  vertex buffer pointer
    void loadData(GLsizei size, GLsizei count, const GLvoid* data);
    void loadInstanceData(unsigned int id, GLsizeiptr size, GLsizei count, const GLvoid* data);
    void loadShaderStorageData(int bindIndex, GLsizei size, const GLvoid* data);
    void loadIndices(GLsizei size, GLsizei count, const GLuint* data);
    const unsigned int initInstanceBuffer(GLenum type = GL_STATIC_DRAW);
    void initVertexBuffer(GLenum type = GL_STATIC_DRAW);
    void initShaderStorageBuffer(GLenum type = GL_STATIC_DRAW);

    void bindVao();
    void unbindVao();

    void bindTexture(GLuint texture);
    void unbindTextures();

    template<typename T>
    void setUniform(const char* name, T value);

    void use();
    void drawArrays(GLenum arrayType = GL_TRIANGLES);
    void drawElements();
    void drawInstances();

    void init();
    ~ShaderProgram();
private:
    template<typename>
    struct GlTypes;

    struct Attribute {
        GLuint  position;
        GLint   dimensions;
        GLenum  type;
        GLsizei size;
        bool    instance;
        GLuint  vbo;
    };

    std::unique_ptr<GLuint>  _programId;
    std::unique_ptr<GLuint>  _vao;
    std::unique_ptr<GLuint>  _vbo;
    std::unique_ptr<GLuint>  _ebo;
    std::unique_ptr<GLuint>  _ssbo;
    std::vector<GLuint>      _instanceVbos;
    std::vector<GLenum>      _instanceVboTypes;
    std::vector<GLuint>      _textures;
    std::vector<GLuint>      _shaders;
    std::vector<Attribute>   _attributes;
    GLsizei                  _numIndices;
    GLsizei                  _numVertices;
    GLsizei                  _numInstances;

    GLenum                   _vboType = GL_STATIC_DRAW;
    GLenum                   _ssboType = GL_STATIC_DRAW;
    
    void  _addShader(GLenum shaderType, const char* data);
    void _defineAttribute(const char* name, GLint dimensions, GLenum type, GLsizei size, 
            bool instance, GLuint vbo);
    void _initSsbo();
    void _initVao();
    void _initVbo();
    void _initInstanceVbo();
    void _initEbo();
    void _bindAttributes(GLuint id);

    GLuint _getOrCreateProgramId();
    GLuint _getUniformLocation(const char* name);
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

template<>
struct ShaderProgram::GlTypes<glm::vec4> {
    static constexpr GLenum type = GL_FLOAT;
};

template<>
struct ShaderProgram::GlTypes<glm::vec2> {
    static constexpr GLenum type = GL_FLOAT;
};

template <typename T>
inline void ShaderProgram::defineAttribute(const char* name, GLint dimensions) 
{
    GLenum type = ShaderProgram::GlTypes<T>::type;
    _defineAttribute(name, dimensions, type, sizeof(T), false, *_vbo);
}

template <typename T>
inline void ShaderProgram::defineInstanceAttribute(unsigned int id, const char* name, GLint dimensions) 
{
    GLenum type = ShaderProgram::GlTypes<T>::type;
    _defineAttribute(name, dimensions, type, sizeof(T), true, _instanceVbos[id]);
}

template <GLsizei S>
inline void ShaderProgram::loadIndices(const GLuint (&data)[S]) {
    loadIndices(S * sizeof(GLuint), S, data);
}

template<GLsizei size, typename T>
inline void ShaderProgram::loadData(T (&data)[size]) 
{
    loadData(size * sizeof(T), size, data);
}

template<GLsizei size, typename T>
inline void ShaderProgram::loadInstanceData(unsigned int id, T (&data)[size]) 
{
    loadInstanceData(id, size * sizeof(T), size, data);
}

template<GLsizei size, typename T>
inline void ShaderProgram::loadShaderStorageData(int bindIndex, T (&data)[size]) 
{
    loadShaderStorageData(bindIndex, size * sizeof(T), size, data);
}


template<>
inline void ShaderProgram::setUniform(const char* name, bool value)
{
    glUniform1i(_getUniformLocation(name), static_cast<int>(value));
}

template<>
inline void ShaderProgram::setUniform(const char*name, int value)
{
    glUniform1i(_getUniformLocation(name), value);
}

template<>
inline void ShaderProgram::setUniform(const char* name, float value)
{
    glUniform1f(_getUniformLocation(name), value);
}

template<>
inline void ShaderProgram::setUniform(const char* name, glm::vec2 value)
{
    glUniform2f(_getUniformLocation(name), value[0], value[1]);
}

template<>
inline void ShaderProgram::setUniform(const char* name, glm::vec4 value)
{
    glUniform4f(_getUniformLocation(name), value[0], value[1], value[2], value[3]);
}

template<>
inline void ShaderProgram::setUniform(const char* name, glm::mat4 value)
{
    glUniformMatrix4fv(_getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

#endif
