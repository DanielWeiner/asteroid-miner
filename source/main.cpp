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
    std::unique_ptr<WindowedApplication> game = std::make_unique<Game>("Asteroid Miner", 2560, 1334);
    Window window(hInst, *game);
    window.run();
    return 0;
}