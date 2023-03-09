#ifndef SHADER_PROGRAM_CONTEXT_H_
#define SHADER_PROGRAM_CONTEXT_H_

#include <glad/glad.h>
#include <vector>

class ShaderProgramContext {
public:
    std::vector<GLuint> instanceVbos;
    std::vector<GLuint> textures;
};

#endif