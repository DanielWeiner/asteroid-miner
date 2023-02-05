#ifndef GAME_H_
#define GAME_H_

#include "../spriteRenderer.h"
#include "outerSpaceScene.h"
#include "drone.h"
#include "../window.h"
#include "../flatScene.h"
#include "../event.h"

#include <memory>
#include <vector>

class Game : public WindowedApplication {
public:
    Game(std::shared_ptr<Window> window);
private:
    std::unique_ptr<OuterSpaceScene>     _outerSpaceScene;
    std::shared_ptr<Window>              _window;
    std::shared_ptr<SpriteRenderer>      _spriteRenderer;
    std::shared_ptr<SpriteRenderer>      _asteroidRenderer;
    std::shared_ptr<FlatScene>           _scene;

    void handleEvent(const Event& event);
    void render();
    void init();
};

#endif