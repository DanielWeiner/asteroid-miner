#include "globalIncludes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "game.h"

#include <memory>

int main()  {
#ifdef _WIN32
}
int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    SetProcessDPIAware();
    timeBeginPeriod(1);
#else
    void* hInst;
#endif
    auto window = std::make_shared<Window>("Asteroid Miner", 2560, 1334, hInst);
    auto game = std::make_shared<Game>(window);
    window->addApplication(game);
    window->run();
    return 0;
}