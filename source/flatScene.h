#ifndef FLAT_SCENE_H_
#define FLAT_SCENE_H_

#include "window.h"

#include <glm/glm.hpp>

class FlatScene {
public:
    FlatScene(const Window& window, float fov);
    
    glm::vec2 getWorldPosition();
    glm::vec2 getWorldSize();

    glm::mat4 getProjection();
    glm::mat4 getView();
    glm::vec2 toWorldCoordinates(glm::vec2 coords);


    void zoomIn(float percent);
    void translate(glm::vec2 xy);
private:
    float _getZpos();
    float _getBaseZpos();
    glm::vec3 _toNdc(glm::vec2 screenCoords);

    const Window&   _window;
    float           _fov;
    float           _zoomLevel = 100.f;
    glm::vec2       _xy = glm::vec2(0.0);
};

#endif