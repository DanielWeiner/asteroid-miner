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
    const int numDrones = 2000;
    const int maxAsteroids = 25;

    Sprite* selectedAsteroid = NULL;
    glm::vec2 selectedAsteroidOffset(0.f, 0.f);

    std::default_random_engine e;
    double random(double peak, double spread)
    {
        std::normal_distribution<double> dis(peak, spread);
        return dis(e);
    }
}

void DemoGame::randomizeDrone(float height, DemoDrone& drone) {
    drone.moveTo(_scene->toWorldCoordinates(glm::vec2(0, height / 2)));
    drone.setSpeed(glm::vec2(random(2, .5), random(0, .5)));
    drone.update();
}

void DemoGame::stepDrone(float width, float height, DemoDrone& drone) {
    auto topLeft = _scene->toWorldCoordinates(glm::vec2(0, 0));
    auto bottomRight = _scene->toWorldCoordinates(_window->getSize());

    drone.step();
    if (drone.getNextPosition().y < topLeft.y || drone.getNextPosition().x < topLeft.x || drone.getNextPosition().y > bottomRight.y || drone.getNextPosition().x > bottomRight.x) {
        randomizeDrone(height, drone);
    }
}

DemoGame::DemoGame(std::shared_ptr<Window> window)
    : _window(window) {}

void DemoGame::handleEvent(const Event &event) {
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
      
                  selectedAsteroidOffset = asteroid->getNextPosition() - glm::vec2(mouse.x, mouse.y);
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
                auto sprite = _spriteFactory->createSprite("Meteors/meteorBrown_big3.png");
                auto dimensions = sprite->getSize();
                sprite->moveTo(mouse.x - dimensions.x / 2, mouse.y - dimensions.y / 2);
    
                _asteroids.push_back(std::move(sprite));
            }
        }
    }

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


void DemoGame::init() 
{
    auto spriteSheet = std::make_shared<SpriteSheet>("data/sprites/sprites.json", "data/sprites/sprites.png");

    spriteSheet->load();
    _spriteFactory = std::make_unique<SpriteFactory>(spriteSheet, _window);
    _scene = std::make_shared<FlatScene>(_window, glm::radians(45.f));

    _spriteRenderer = _spriteFactory->createRenderer();
    _lineRenderer = std::make_unique<LineRenderer>(_window);
    _lineRenderer->init();

    const char* names[] = {
        "playerShip1_blue.png",
        "playerShip1_green.png",
        "playerShip1_orange.png",
        "playerShip1_red.png",
        "playerShip2_blue.png",
        "playerShip2_green.png",
        "playerShip2_orange.png",
        "playerShip2_red.png",
        "playerShip3_blue.png",
        "playerShip3_green.png",
        "playerShip3_orange.png",
        "playerShip3_red.png"
    };

    for (int i = 0; i < numDrones; i++) {
        auto spriteName = names[rand() % (sizeof(names) / sizeof(char*))];
        auto sprite = _spriteFactory->createSprite(spriteName);
        sprite->scaleBy(glm::vec2(0.5));

        auto drone = std::make_unique<DemoDrone>(sprite);

        randomizeDrone(_window->getSize().y, *drone);
        for (int i = rand() % 250; i >= 0; i--) {
            stepDrone(_window->getSize().x, _window->getSize().y, *drone);
        }

        _drones.push_back(std::move(drone));
    }
}

void DemoGame::render() 
{
    for (auto& asteroid : _asteroids) {
        asteroid->update();
    }
    
    for (auto& drone : _drones) {
        for (auto& asteroid : _asteroids) {
            auto dronePosition = drone->getNextPosition() + (drone->getSize() / 2.f);
            auto asteroidPosition = asteroid->getNextPosition() + (asteroid->getSize() / 2.f);
            auto distance = glm::distance(dronePosition, asteroidPosition);
            float pull = 50.f / distance;
            auto diff = asteroidPosition - dronePosition;
            auto unit = diff * (1.f / distance);

            drone->setSpeed(drone->getSpeed() + unit * pull);
        }

        stepDrone(_window->getSize().x, _window->getSize().y, *drone);
    }
    _spriteRenderer->setProjection(_scene->getProjection());
    _spriteRenderer->setView(_scene->getView());
    _spriteRenderer->draw();
}