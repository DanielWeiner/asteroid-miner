#include "game.h"

#include "../spriteFactory.h"
#include "../spriteSheet.h"
#include "droneFactory.h"
#include <glm/glm.hpp>

Game::Game(std::shared_ptr<Window> window)
    : _window(window) {}

void Game::handleEvent(const Event &event) {
    if (event.action == EventAction::SCROLL) {
      _scene->zoomIn(10.f * (float)event.yoffset);
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
                _scene->translate(glm::vec2(-x, -y) * 50.f);
            }
        }
    }
}


void Game::init() 
{
    auto spriteSheet = std::make_shared<SpriteSheet>("data/sprites/sprites.json", "data/sprites/sprites.png");
    spriteSheet->load();

    _scene = std::make_shared<FlatScene>(_window, glm::radians(45.f));

    auto spriteFactory = std::make_unique<SpriteFactory>(spriteSheet, _window);
    auto asteroidSpriteFactory = std::make_unique<SpriteFactory>(spriteSheet, _window);
    auto uiSpriteFactory = std::make_unique<SpriteFactory>(spriteSheet, _window);

    _spriteRenderer = spriteFactory->createRenderer();
    _asteroidRenderer = asteroidSpriteFactory->createRenderer();

    auto asteroidSpawner = std::make_unique<AsteroidSpawner>(std::move(asteroidSpriteFactory), _scene);
    auto droneFactory = std::make_unique<DroneFactory>(std::move(spriteFactory));

    _outerSpaceScene = std::make_unique<OuterSpaceScene>(std::move(droneFactory), std::move(asteroidSpawner), _scene);
    _scene->zoomIn(-100.f);
    _outerSpaceScene->init();

    _lineRenderer = std::make_unique<LineRenderer>(_window);
    _lineRenderer->init();

    _outerSpaceUi = std::make_unique<OuterSpaceUi>(std::move(uiSpriteFactory), _window);

    _outerSpaceUi->init();
    _outerSpaceUi->setFontSize(2);
    _outerSpaceUi->setPanelBounds(glm::vec2(200.f,50.f), glm::vec2(1250.f,1000.f));
    _outerSpaceUi->setFontColor(glm::vec4(0.2, 0.2, 0.2, 1.0));
    _outerSpaceUi->setText(
        R"(Apparently motionless to her passengers and crew, the Interplanetary liner Hyperion bored serenely onward through space at normal acceleration. In the railed-off sanctum in one corner of the control room a bell tinkled, a smothered whirr was heard, and Captain Bradley frowned as he studied the brief message upon the tape of the recorder--a message flashed to his desk from the operator's panel. He beckoned, and the second officer, whose watch it now was, read aloud: "Reports of scout patrols still negative." "Still negative." The officer scowled in thought. "They've already searched beyond the widest possible location of the wreckage, too. Two unexplained disappearances inside a month--first the Dione, then the Rhea--and not a plate nor a lifeboat recovered. Looks bad, sir. One might be an accident; two might possibly be a coincidence...." His voice died away. What might that coincidence mean?)"
    );
}

void Game::render() 
{

    const int numRenderers = 2;

    _outerSpaceScene->step();
    

    std::shared_ptr<SpriteRenderer> renderers[numRenderers] = {
        _asteroidRenderer,
        _spriteRenderer
    };
    
    for (int i = 0; i < numRenderers; i++) {
        renderers[i]->setProjection(_scene->getProjection());
        renderers[i]->setView(_scene->getView());
        renderers[i]->draw();
    }
    _lineRenderer->setProjection(_scene->getProjection());
    _lineRenderer->setView(_scene->getView());

    _outerSpaceScene->render(*_lineRenderer);
    _outerSpaceUi->render();
}