#ifndef LINE_RENDERER_H_
#define LINE_RENDERER_H_

#include <memory>
#include <vector>
#include <glm/glm.hpp>

#include "window.h"

class ShaderProgram;

class LineRenderer {
public:
    LineRenderer(std::shared_ptr<Window> window);
    void init();
    void setProjection(glm::mat4 projection);
    void setView(glm::mat4 view);
    void lineTo(glm::vec2 from, glm::vec2 to, glm::vec4 color, float width = 1);
private:
    static constexpr unsigned int  VERTEX_SIZE = 2;
    std::unique_ptr<ShaderProgram> _shaderProgram;
    std::shared_ptr<Window>        _window;
    std::vector<float>             _vertices;
    glm::mat4                      _projection;
    glm::mat4                      _view;
};

#endif