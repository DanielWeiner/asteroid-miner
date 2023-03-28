#ifndef GAME_H_
#define GAME_H_

#include "../spriteRenderer.h"
#include "outerSpaceScene.h"
#include "drone.h"
#include "../window.h"
#include "../flatScene.h"
#include "../event.h"
#include "../lineRenderer.h"

#include <vector>

class Game : public WindowedApplication {
public:
    Game(
        Window& window,
        SpriteSheet& spriteSheet,
        SpriteRenderer& spriteRenderer,
        SpriteRenderer& uiSpriteRenderer, 
        SpriteRenderer& asteroidSpriteRenderer, 
        DroneFactory& droneFactory,
        FlatScene& flatScene
    );
private:
    Window&         _window;
    SpriteRenderer& _spriteRenderer;
    SpriteRenderer& _asteroidSpriteRenderer;
    LineRenderer    _lineRenderer;
    OuterSpaceScene _outerSpaceScene;
    FlatScene&      _scene;
    
    void handleEvent(const Event& event);
    void render();
    void init();
};

#endif