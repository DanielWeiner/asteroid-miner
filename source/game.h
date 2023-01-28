#ifndef GAME_H_
#define GAME_H_

#include "spriteFactory.h"
#include "spriteRenderer.h"
#include "game/drone.h"
#include "windowedApplication.h"

#include <memory>
#include <vector>


class Game : public WindowedApplication {
public:
    Game(const char* name, float width, float height);
private:
    std::unique_ptr<SpriteFactory>       _spriteFactory;
    std::shared_ptr<SpriteRenderer>      _renderer;
    float                                _width;
    float                                _height;
    std::string                          _displayName;
    std::vector<std::unique_ptr<Drone>>  _drones;
    std::vector<std::unique_ptr<Sprite>> _asteroids;

    void handleEvent(const Event& event) override;
    void render() override;
    void init() override;
    const char* getName() override;
    float getWidth() override;
    float getHeight() override;
};

#endif