#include "flatScene.h"

#include <glm/ext.hpp>

FlatScene::FlatScene(const FlatScene& other)
: _window(other._window), _fov(other._fov)
{
}

FlatScene::FlatScene(Window& window, float fov) : 
_window(window), _fov(fov) {}

glm::vec2 FlatScene::getWorldPosition()
{
    return toWorldCoordinates(glm::vec2(0,0));
}

glm::vec2 FlatScene::getWorldSize()
{
    auto size = _window.getSize();
    return size * _getZpos() / _getBaseZpos();
}

glm::mat4 FlatScene::getProjection()
{
    auto size = _window.getSize();

    glm::mat4 projection = glm::perspective(_fov, size.x / size.y, -1.0f, 1.0f);
    
    return projection;
}

glm::mat4 FlatScene::getView()
{
    auto size = _window.getSize();

    glm::mat4 view(1.0);
    view = glm::rotate(view, glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
    view = glm::translate(view, glm::vec3(-size.x / 2 + _xy.x, -size.y / 2 + _xy.y, _getZpos()));
    
    return view;
}

glm::vec2 FlatScene::toWorldCoordinates(glm::vec2 coords)
{
    glm::mat4 screenModel(1.0);
    
    screenModel = glm::translate(screenModel, glm::vec3(-_xy, 0.f));
    screenModel = glm::scale(screenModel, glm::vec3(_window.getSize() / 2.f, 1.f));
    screenModel = glm::translate(screenModel, glm::vec3(1.f));
    screenModel = glm::scale(screenModel, glm::vec3(1.f, -1.f, 1.f));
    screenModel = glm::scale(screenModel, glm::vec3(_getZpos() / _getBaseZpos()));

    return screenModel * glm::vec4(_toNdc(coords), 1.f);
}

void FlatScene::zoomIn(float percent) 
{
    _zoomLevel -= percent;
    _zoomLevel = glm::max(0.f, glm::min(1000.f, _zoomLevel));
}

void FlatScene::translate(glm::vec2 xy) 
{
    _xy += xy;
}

float FlatScene::_getZpos()
{
    return _zoomLevel / 100.f * _getBaseZpos();
}

float FlatScene::_getBaseZpos()
{
    auto size = _window.getSize();
    auto distanceToEdge = (size.y / 2) / glm::sin(_fov / 2);
    return glm::sqrt(glm::pow(distanceToEdge, 2) - glm::pow(size.y / 2, 2));
}

glm::vec3 FlatScene::_toNdc(glm::vec2 screenCoords)
{
     auto size = _window.getSize();
    return (glm::vec3(screenCoords.x, size.y - screenCoords.y, 0) / glm::vec3(size, 1)) * 2.f - 1.f;
}
