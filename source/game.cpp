#include "game.h"
#include "event.h"

#include <string>
#include <random>

namespace {
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
: Game(new SpriteRenderer(width, height), name, width, height) 
{
}

Game::Game(SpriteRenderer *renderer, const char* name, float width, float height)
    : _renderer(renderer),  _width(width), _height(height), _displayName(name) {}

void Game::handleEvent(const Event &event) {
  if (event.action == EventAction::RESIZE) {
    _width = event.width;
    _height = event.height;
    _displayName = "(" + to_string((int)_width) + ", " + to_string((int)_height)+ ")";
    _renderer->updateDimensions(_width, _height);
  }
}


void Game::init() 
{
    const int numSprites = 200;
    
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

    _renderer->init();
    _renderer->setBuffer(numSprites);

    for (int i = 0; i < numSprites; i++) {
        auto spriteName = names[rand() % (sizeof(names) / sizeof(char*))];
        auto sprite = _renderer->createSprite(spriteName);
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
