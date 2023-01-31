#include "game.h"

#include "window.h"

#include "spriteFactory.h"
#include "spriteBuffer.h"
#include "spriteSheet.h"
#include "spriteRenderer.h"
#include "game/drone.h"
#include "event.h"
#include "sprite.h"

#include <string>
#include <random>
#include <iostream>
#include <glm/ext.hpp>
#include <GLFW/glfw3.h>

namespace {
    const int numDrones = 5000;
    const int maxAsteroids = 25;

    Sprite* selectedAsteroid = NULL;
    glm::vec2 selectedAsteroidOffset(0.f, 0.f);

    std::default_random_engine e;
    double random(double peak, double spread)
    {
        std::normal_distribution<double> dis(peak, spread);
        return dis(e);
    }

    template<int size, typename T, glm::qualifier Q>
    void printVector(glm::vec<size, T, Q> vector) {
        std::cout << "(";
        for (int i = 0; i < size; i++) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << vector[i];
        }
        std::cout << ")\n";
    }
}

void Game::randomizeDrone(float height, Drone& drone) {
    drone.moveTo(_toWorldCoordinates(glm::vec2(0, height / 2)));
    drone.setSpeed(glm::vec2(random(2, .5), random(0, .5)));
}

void Game::stepDrone(float width, float height, Drone& drone) {
    auto topLeft = _toWorldCoordinates(glm::vec2(0, 0));
    auto bottomRight = _toWorldCoordinates(_window->getSize());

    drone.step();
    if (drone.getPosition().y < topLeft.y || drone.getPosition().x < topLeft.x || drone.getPosition().y > bottomRight.y || drone.getPosition().x > bottomRight.x) {
        randomizeDrone(height, drone);
    }
}

Game::Game(std::shared_ptr<Window> window)
    : _window(window) {}

glm::vec3 Game::_toNdc(glm::vec2 coords)
{
    auto size = _window->getSize();
    return (glm::vec3(coords.x, size.y - coords.y, 0) / glm::vec3(size, 1)) * 2.f - 1.f;
}

glm::mat4 Game::_getProjection()
{
    auto size = _window->getSize();

    glm::mat4 projection = glm::perspective(_fov, size.x / size.y, -1.0f, 1.0f);
    
    return projection;
}

glm::mat4 Game::_getView()
{
    auto size = _window->getSize();

    glm::mat4 view(1.0);
    view = glm::rotate(view, glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
    view = glm::translate(view, glm::vec3(-size.x / 2, -size.y / 2, _zpos));
    
    return view;
}

glm::vec2 Game::_toWorldCoordinates(glm::vec2 coords)
{
    glm::mat4 screenModel(1.0);
    screenModel = glm::scale(screenModel, glm::vec3(_window->getSize() / 2.f, 1.f));
    screenModel = glm::translate(screenModel, glm::vec3(1.f));
    screenModel = glm::scale(screenModel, glm::vec3(1.f, -1.f, 1.f));
    screenModel = glm::scale(screenModel, glm::vec3(_zpos / _getBaseZpos(_fov)));

    return screenModel * glm::vec4(_toNdc(coords), 1.f);
}

float Game::_getBaseZpos(float fov)
{
    auto size = _window->getSize();
    auto distanceToEdge = (size.y / 2) / glm::sin(fov / 2);
    return glm::sqrt(glm::pow(distanceToEdge, 2) - glm::pow(size.y / 2, 2));
}

void Game::handleEvent(const Event &event) {
    if (event.action == EventAction::RESIZE) {
        _window->setTitle("(" + to_string((int)event.width) + ", " + to_string((int)event.height)+ ")");
    }
  
    if (event.type == EventType::MOUSE && event.action == EventAction::PRESS) {
        auto mouse = _toWorldCoordinates(glm::vec2(event.x, event.y));
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
            auto mouse = _toWorldCoordinates(glm::vec2(event.x, event.y));
            selectedAsteroid->moveTo(glm::vec2(mouse.x, mouse.y) + selectedAsteroidOffset);
        }
    }

    if (event.action == EventAction::CLICK) {
        auto mouse = _toWorldCoordinates(glm::vec2(event.x, event.y));
        if (_asteroids.size() < maxAsteroids && event.button == GLFW_MOUSE_BUTTON_1) {
            bool inHitbox = false;
            for (auto& asteroid : _asteroids) {
                if (asteroid->pointIsInHitbox(mouse.x, mouse.y)) {
                    inHitbox = true;
                    break;
                }
            }
    
            if (!inHitbox) {
                auto sprite = _spriteFactory->createSprite("Enemies/enemyGreen3.png");
                auto dimensions = sprite->getSize();
                sprite->moveTo(mouse.x - dimensions.x / 2, mouse.y - dimensions.y / 2);
    
                _asteroids.push_back(std::move(sprite));
            }
        }
    }

    if (event.action == EventAction::SCROLL) {
      _zpos -= 100.f * (float)event.yoffset;
    }
}


void Game::init() 
{
    _fov = glm::radians(45.f);
    _zpos = _getBaseZpos(_fov);

    auto spriteBuffer = std::make_shared<SpriteBuffer>();
    auto spriteSheet = std::make_shared<SpriteSheet>("data/sprites/sprites.json", "data/sprites/sprites.png");
    spriteSheet->load();
    _spriteFactory = std::make_unique<SpriteFactory>(spriteSheet, spriteBuffer, _window);
    _spriteRenderer = _spriteFactory->createRenderer();
    _lineRenderer = std::make_unique<LineRenderer>(_window);
    _spriteRenderer->init();

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
        "playerShip3_red.png",
        "ufoBlue.png",
        "ufoGreen.png",
        "ufoRed.png",
        "ufoYellow.png"
    };

    for (int i = 0; i < numDrones; i++) {
        auto spriteName = names[rand() % (sizeof(names) / sizeof(char*))];
        auto sprite = _spriteFactory->createSprite(spriteName);
        sprite->scaleBy(glm::vec2(0.5));

        auto drone = std::make_unique<Drone>(sprite);

        randomizeDrone(_window->getSize().y, *drone);
        for (int i = rand() % 250; i >= 0; i--) {
            stepDrone(_window->getSize().x, _window->getSize().y, *drone);
        }

        _drones.push_back(std::move(drone));
    }
}

void Game::render() 
{
    for (auto& drone : _drones) {
        for (auto& asteroid : _asteroids) {
            auto dronePosition = drone->getPosition() + (drone->getSize() / 2.f);
            auto asteroidPosition = asteroid->getPosition() + (asteroid->getSize() / 2.f);
            auto distance = glm::distance(dronePosition, asteroidPosition);
            float pull = 50.f / distance;
            auto diff = asteroidPosition - dronePosition;
            auto unit = diff * (1.f / distance);

            drone->setSpeed(drone->getSpeed() + unit * pull);
        }

        stepDrone(_window->getSize().x, _window->getSize().y, *drone);
    }
    _spriteRenderer->setProjection(_getProjection());
    _spriteRenderer->setView(_getView());
    _spriteRenderer->draw();
}