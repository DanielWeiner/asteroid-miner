#ifndef GAME_H_
#define GAME_H_

#include "lineRenderer.h"
#include "sprite.h"
#include "spriteFactory.h"
#include "spriteRenderer.h"
#include "game/drone.h"
#include "window.h"

#include <memory>
#include <vector>

class Game : public WindowedApplication {
public:
    Game(std::shared_ptr<Window> window);
private:
    std::shared_ptr<Window>              _window;
    std::unique_ptr<SpriteFactory>       _spriteFactory;
    std::shared_ptr<SpriteRenderer>      _spriteRenderer;
    std::unique_ptr<LineRenderer>        _lineRenderer;
    std::vector<std::unique_ptr<Drone>>  _drones;
    std::vector<std::unique_ptr<Sprite>> _asteroids;
    float                                _fov;
    float                                _zpos;

    void stepDrone(float width, float height, Drone& drone);
    void randomizeDrone(float height, Drone& drone);
    glm::vec3 _toNdc(glm::vec2 coords);
    glm::mat4 _getProjection();
    glm::mat4 _getView();
    glm::vec2 _toWorldCoordinates(glm::vec2 screenCoords);
    float     _getBaseZpos(float fov);
    void handleEvent(const Event& event);
    void render();
    void init();
};

#endif