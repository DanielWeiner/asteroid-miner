#include "game.h"

#include "../spriteFactory.h"
#include "../spriteBuffer.h"
#include "../spriteSheet.h"
#include "../event.h"
#include "../sprite.h"

#include <string>
#include <random>
#include <iostream>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

namespace {
    const int maxAsteroids = 25;

    Sprite* selectedAsteroid = NULL;
    glm::vec2 selectedAsteroidOffset(0.f, 0.f);
}

Game::Game(std::shared_ptr<Window> window)
    : _window(window) {}

void Game::handleEvent(const Event &event) {
    if (event.action == EventAction::RESIZE) {
        _window->setTitle("(" + to_string((int)event.width) + ", " + to_string((int)event.height)+ ")");
    }
  
    if (event.type == EventType::MOUSE && event.action == EventAction::PRESS) {
        auto mouse = _scene->toWorldCoordinates(glm::vec2(event.x, event.y));
        int i = _asteroids.size();
        for (auto it = _asteroids.rbegin(); it < _asteroids.rend(); it++) {
            i--;
            auto& asteroid = *it;
            
            if (asteroid->pointIsInHitbox(mouse.x, mouse.y)) {
                  if (event.button == GLFW_MOUSE_BUTTON_2) {
                      _asteroids.erase(_asteroids.begin() + i);
                      selectedAsteroid = NULL;
                      break;
                  }
      
                  selectedAsteroidOffset = asteroid->getPosition() - glm::vec2(mouse.x, mouse.y);
                  auto newSprite = _spriteFactory->createSprite(asteroid->getName());
                  *newSprite = *asteroid;
                  selectedAsteroid = newSprite.get();
      
                  _asteroids.erase(_asteroids.begin() + i);
                  _asteroids.push_back(std::move(newSprite));
                  break;
            }
            
        }
    }
  
    if (event.action == EventAction::RELEASE) {
        selectedAsteroid = NULL;
    }
  
    if (event.action == EventAction::HOVER || event.action == EventAction::REPEAT) {
        if (selectedAsteroid != NULL) {
            auto mouse = _scene->toWorldCoordinates(glm::vec2(event.x, event.y));
            selectedAsteroid->moveTo(glm::vec2(mouse.x, mouse.y) + selectedAsteroidOffset);
        }
    }

    if (event.action == EventAction::CLICK) {
        auto mouse = _scene->toWorldCoordinates(glm::vec2(event.x, event.y));
        if (_asteroids.size() < maxAsteroids && event.button == GLFW_MOUSE_BUTTON_1) {
            bool inHitbox = false;
            for (auto& asteroid : _asteroids) {
                if (asteroid->pointIsInHitbox(mouse.x, mouse.y)) {
                    inHitbox = true;
                    break;
                }
            }
    
            if (!inHitbox) {
                auto sprite = _spriteFactory->createSprite("Meteors/spaceMeteors_004.png");
                auto dimensions = sprite->getSize();
                sprite->moveTo(mouse.x - dimensions.x / 2, mouse.y - dimensions.y / 2);
    
                _asteroids.push_back(std::move(sprite));
            }
        }
    }

    if (event.action == EventAction::SCROLL) {
      _scene->zoomIn(-10.f * (float)event.yoffset);
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
    auto spriteBuffer = std::make_shared<SpriteBuffer>();
    auto spriteSheet = std::make_shared<SpriteSheet>("data/sprites/sprites.json", "data/sprites/sprites.png");
    spriteSheet->load();
    _spriteFactory = std::make_shared<SpriteFactory>(spriteSheet, spriteBuffer, _window);
    
    _scene = std::make_shared<FlatScene>(_window, glm::radians(45.f));

    _spriteRenderer = _spriteFactory->createRenderer();
    _lineRenderer = std::make_unique<LineRenderer>(_window);
    _spriteRenderer->init();
    _lineRenderer->init();

    _asteroidSpawner = std::make_unique<AsteroidSpawner>(_spriteFactory, _scene);
    _asteroidSpawner->step();
}

void Game::render() 
{
    for (auto& smallAsteroid : _asteroidSpawner->asteroids()) {
        for (auto& asteroid : _asteroids) {
            auto smallAsteroidPosition = smallAsteroid->getPosition() + (smallAsteroid->getSize() / 2.f);
            auto asteroidPosition = asteroid->getPosition() + (asteroid->getSize() / 2.f);
            auto distance = glm::distance(smallAsteroidPosition, asteroidPosition);
            float pull = 50.f / distance;
            auto diff = asteroidPosition - smallAsteroidPosition;
            auto unit = diff * (1.f / distance);

            smallAsteroid->setSpeed(smallAsteroid->getSpeed() + unit * pull);
        }
    }

    _asteroidSpawner->step();
    _spriteRenderer->setProjection(_scene->getProjection());
    _spriteRenderer->setView(_scene->getView());
    _spriteRenderer->draw();
}