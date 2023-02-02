#ifndef DEMO_GAME_H_
#define DEMO_GAME_H_

#include "../lineRenderer.h"
#include "../sprite.h"
#include "../spriteFactory.h"
#include "../spriteRenderer.h"
#include "../game/drone.h"
#include "../window.h"
#include "../flatScene.h"

#include <memory>
#include <vector>

class DemoGame : public WindowedApplication {
public:
    DemoGame(std::shared_ptr<Window> window);
private:
    std::shared_ptr<Window>              _window;
    std::unique_ptr<SpriteFactory>       _spriteFactory;
    std::shared_ptr<SpriteRenderer>      _spriteRenderer;
    std::unique_ptr<LineRenderer>        _lineRenderer;
    std::vector<std::unique_ptr<Drone>>  _drones;
    std::vector<std::unique_ptr<Sprite>> _asteroids;
    std::shared_ptr<FlatScene>           _scene;

    void stepDrone(float width, float height, Drone& drone);
    void randomizeDrone(float height, Drone& drone);
    void handleEvent(const Event& event);
    void render();
    void init();
};

#endif