#ifndef GAME_H_
#define GAME_H_

#include "../lineRenderer.h"
#include "../sprite.h"
#include "../spriteFactory.h"
#include "../spriteRenderer.h"
#include "drone.h"
#include "../window.h"
#include "../flatScene.h"
#include "asteroidSpawner.h"

#include <memory>
#include <vector>

class Game : public WindowedApplication {
public:
    Game(std::shared_ptr<Window> window);
private:
    std::shared_ptr<Window>              _window;
    std::shared_ptr<SpriteFactory>       _spriteFactory;
    std::shared_ptr<SpriteRenderer>      _spriteRenderer;
    std::unique_ptr<LineRenderer>        _lineRenderer;
    std::vector<std::unique_ptr<Drone>>  _drones;
    std::vector<std::unique_ptr<Sprite>> _asteroids;
    std::shared_ptr<FlatScene>           _scene;
    std::unique_ptr<AsteroidSpawner>     _asteroidSpawner;

    void stepDrone(float width, float height, Drone& drone);
    void randomizeDrone(float height, Drone& drone);
    void handleEvent(const Event& event);
    void render();
    void init();
};

#endif