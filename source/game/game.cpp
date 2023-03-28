#include "game.h"

#include "../spriteFactory.h"
#include "../spriteSheet.h"
#include "droneFactory.h"
#include <glm/glm.hpp>

Game::Game(
    Window& window,
    SpriteSheet& spriteSheet,
    SpriteRenderer& spriteRenderer,
    SpriteRenderer& uiSpriteRenderer,
    SpriteRenderer& asteroidSpriteRenderer, 
    DroneFactory& droneFactory,
    FlatScene& flatScene
)
: _window(window), 
  _spriteRenderer(spriteRenderer),
  _asteroidSpriteRenderer(asteroidSpriteRenderer),
  _outerSpaceScene(droneFactory, asteroidSpriteRenderer, flatScene),
  _scene(flatScene)
{
}



void Game::handleEvent(const Event &event) {
    if (event.action == EventAction::SCROLL) {
      _scene.zoomIn(10.f * (float)event.yoffset);
    }

    if (event.type == EventType::KEY) {
        if (event.action == EventAction::PRESS || event.action == EventAction::REPEAT) {
            float x = 0.0f, y = 0.0f;
            switch (event.key) {
            case Key::KEY_A:
                x = -1.f;
                break;
            case Key::KEY_D:
                x = 1.f;
                break;
            case Key::KEY_W:
                y = -1.f;
                break;
            case Key::KEY_S:
                y = 1.f;
                break;
            }
            if (x != 0.f || y != 0.f) {
                _scene.translate(glm::vec2(-x, -y) * 50.f);
            }
        }
    }
}


void Game::init() 
{
    _scene.zoomIn(-100.f);
    _outerSpaceScene.init();
    _lineRenderer.init();
}

void Game::render() 
{
    _outerSpaceScene.step();

    SpriteRenderer renderers[] = {
        _asteroidSpriteRenderer,
        _spriteRenderer
    };
    
    for (int i = 0; i < sizeof(renderers)/sizeof(SpriteRenderer); i++) {
        renderers[i].setProjection(_scene.getProjection());
        renderers[i].setView(_scene.getView());
        renderers[i].draw();
    }
    _lineRenderer.setProjection(_scene.getProjection());
    _lineRenderer.setView(_scene.getView());

    _outerSpaceScene.render(_lineRenderer);
}