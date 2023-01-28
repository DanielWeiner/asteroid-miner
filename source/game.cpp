#include "game.h"

#include "spriteBuffer.h"
#include "spriteSheet.h"
#include "spriteRenderer.h"
#include "game/drone.h"
#include "event.h"
#include "sprite.h"

#include <string>
#include <random>

namespace {
    const int numDrones = 500;
    const int maxAsteroids = 25;

    Sprite* selectedAsteroid = NULL;

    std::default_random_engine e;
    double random(double peak, double spread)
    {
        std::normal_distribution<double> dis(peak, spread);
        return dis(e);
    }

    void randomizeDrone(float height, Drone& drone) {
        drone.moveTo(glm::vec2(0, height / 2));
        drone.setSpeed(glm::vec2(random(7, .5), random(0, .5)));
    }

    void stepDrone(float width, float height, Drone& drone) {
        drone.step();
        if (drone.getPosition().y > height || drone.getPosition().x > width) {
            randomizeDrone(height, drone);
        }
    }
}

Game::Game(const char* name, float width, float height)
    : _width(width), _height(height), _displayName(name) {}

void Game::handleEvent(const Event &event) {
  if (event.action == EventAction::RESIZE) {
    _width = event.width;
    _height = event.height;
    _displayName = "(" + to_string((int)_width) + ", " + to_string((int)_height)+ ")";
    _renderer->updateDimensions(_width, _height);
  }

  if (event.type == EventType::MOUSE && event.action == EventAction::PRESS) {
    int i = 0;
    for (auto& asteroid : _asteroids) {
        if (asteroid->pointIsInHitbox(event.x, event.y)) {
            auto newSprite = _spriteFactory->createSprite(asteroid->getName());
            *newSprite = *asteroid;
            _asteroids.erase(_asteroids.begin() + i);
            selectedAsteroid = newSprite.get();
            auto dimensions = selectedAsteroid->getRawDimensions();
            selectedAsteroid->moveTo(event.x - dimensions.x / 2, event.y - dimensions.y / 2);
            _asteroids.push_back(std::move(newSprite));
            break;
        }
        i++;
    }
  }

  if (event.action == EventAction::RELEASE) {
    selectedAsteroid = NULL;
  }

  if (event.action == EventAction::HOVER || event.action == EventAction::REPEAT) {
    if (selectedAsteroid != NULL) {
        auto dimensions = selectedAsteroid->getRawDimensions();
        selectedAsteroid->moveTo(event.x - dimensions.x / 2, event.y - dimensions.y / 2);
    }
  }

  if (event.action == EventAction::CLICK) {
    if (_asteroids.size() < maxAsteroids) {
        auto sprite = _spriteFactory->createSprite("Enemies/enemyGreen3.png");
        auto dimensions = sprite->getRawDimensions();
        sprite->moveTo(event.x - dimensions.x / 2, event.y - dimensions.y / 2);

        _asteroids.push_back(std::move(sprite));
    }
  }
}


void Game::init() 
{   
    auto spriteBuffer = std::make_shared<SpriteBuffer>();
    auto spriteSheet = std::make_shared<SpriteSheet>("data/sprites/sprites.json", "data/sprites/sprites.png");
    spriteSheet->load();
    _spriteFactory = std::make_unique<SpriteFactory>(spriteSheet, spriteBuffer);
    _renderer = _spriteFactory->createRenderer(_width, _height);
    _renderer->init();

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

        randomizeDrone(_height, *drone);
        for (int i = rand() % 250; i >= 0; i--) {
            stepDrone(_width, _height, *drone);
        }

        _drones.push_back(std::move(drone));
    }
}

void Game::render() 
{
    _renderer->clearScreen();
    for (auto& drone : _drones) {
        stepDrone(_width, _height, *drone);
    }
    _renderer->draw();
}

const char* Game::getName()
{
    return _displayName.c_str();
}

float Game::getWidth()
{
    return _width;
}

float Game::getHeight()
{
    return _height;
}
