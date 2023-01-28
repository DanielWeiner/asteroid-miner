#ifndef GAME_H_
#define GAME_H_

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
    std::shared_ptr<SpriteRenderer>      _renderer;
    std::vector<std::unique_ptr<Drone>>  _drones;
    std::vector<std::unique_ptr<Sprite>> _asteroids;

    void handleEvent(const Event& event);
    void render();
    void init();
    const char* getName();
    float getWidth();
    float getHeight();
};

#endif