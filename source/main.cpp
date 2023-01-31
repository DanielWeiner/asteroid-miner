#include "global.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "winConsole.h"
#include "window.h"
#include "game.h"
#include <iostream>
#include <memory>
#include <filesystem>


int main(int argc, char** argv) 
{
    auto str = std::filesystem::weakly_canonical(std::filesystem::path(argv[0])).parent_path().string();
    Global::SetExeDir(str.c_str());
#ifdef _WIN32
    SetProcessDPIAware();
    timeBeginPeriod(1);
    WinConsole::CreateNewConsole();
#endif
    std::cout << "hello";
    auto window = std::make_shared<Window>("Asteroid Miner", 2560, 1334);
    auto game = std::make_shared<Game>(window);
    window->addApplication(game);
    window->run();
#ifdef _WIN32
    WinConsole::ReleaseConsole();
#endif
    exit(0);
}

