#include "global.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "spriteSheet.h"
#include "util/range.h"
#include "winConsole.h"
#include "window.h"
#include "game/game.h"
#include "game/collisionTester.h"
#include <memory>
#include <filesystem>


int main(int argc, char** argv) 
{
    auto str = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path().string();
    Global::SetExeDir(str.c_str());
#ifdef _WIN32
    SetProcessDPIAware();
    timeBeginPeriod(2);
    WinConsole::CreateNewConsole();
#endif
    Window window("Asteroid Miner", 2560, 1334);
    window.init();

    SpriteSheet spriteSheet("data/sprites/sprites.json", "data/sprites/sprites.png");
    SpriteFactory spriteFactory(window, spriteSheet);
    SpriteRenderer* spriteRenderer = spriteFactory.createRenderer();
    SpriteRenderer* asteroidSpriteRenderer = spriteFactory.createRenderer();
    SpriteRenderer* uiSpriteRenderer = spriteFactory.createRenderer(false);

    DroneFactory droneFactory(*spriteRenderer);
    FlatScene flatScene(window, glm::radians(45.f));

    Game game(window, spriteSheet, *spriteRenderer, *uiSpriteRenderer, *asteroidSpriteRenderer, droneFactory, flatScene);
    window.addApplication(&game);
    window.run();
#ifdef _WIN32
    WinConsole::ReleaseConsole();
#endif
    exit(0);
}

