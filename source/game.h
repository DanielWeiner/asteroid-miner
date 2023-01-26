#ifndef GAME_H_
#define GAME_H_

#include "game/drone.h"
#include "window.h"
#include "windowedApplication.h"
#include "sprite.h"
#include "spriteRenderer.h"

#include <memory>
#include <vector>

class Game : public WindowedApplication {
public:
    Game(const char* name, float width, float height);
private:
    Game(SpriteRenderer* renderer, const char* name, float width, float height);
    
    std::unique_ptr<SpriteRenderer>      _renderer;
    float                                _width;
    float                                _height;
    std::string                          _displayName;
    std::vector<std::unique_ptr<Drone>>  _drones;

    void handleEvent(const Event& event) override;
    void render() override;
    void init() override;
    const char* getName() override;
    float getWidth() override;
    float getHeight() override;
};

#endif