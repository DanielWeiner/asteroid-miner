#ifndef LINE_RENDERER_H_
#define LINE_RENDERER_H_

#include <memory>
#include <vector>
#include <glm/glm.hpp>

class ShaderProgram;

class LineRenderer {
public:
    void init();
    void lineTo(glm::vec2 from, glm::vec2 to);
    void draw();
private:
    using VertexColor = glm::vec<6, float>;
    
    std::unique_ptr<ShaderProgram> _shaderProgram;
    std::vector<VertexColor>       _vertices;
};

#endif