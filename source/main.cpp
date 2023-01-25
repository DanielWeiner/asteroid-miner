#include "globalIncludes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "sprite.h"
#include "event.h"
#include "spriteRenderer.h"

#include <random>

int main()  {
#ifdef _WIN32
}
int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    SetProcessDPIAware();
    timeBeginPeriod(1);
#endif

	Window window = Window("Asteroid Miner", { 640, 480 });
    ShaderProgram program;
    SpriteRenderer spriteRenderer(&program, &window);
	window.setEventLoop([&window](const Event &event) {
        if (event.action == EventAction::RESIZE) {
            auto dimensions = window.getDimensions();
            window.setName("(" + to_string(dimensions.x) + ", " + to_string(dimensions.y) + ")");
        }
	});
    
    const int numSprites = 1000;
    std::vector<std::shared_ptr<Sprite>> sprites;
    
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

    window.onInit([&spriteRenderer, &sprites, &names]() {
        spriteRenderer.init();
        for (int i = 0; i < numSprites; i++) {
            auto spriteName = names[rand() % (sizeof(names) / sizeof(char*))];
            auto sprite = spriteRenderer.createSprite(spriteName);
            sprite->moveTo(500,500);
            sprites.push_back(sprite);
        }
    });

    window.setRenderLoop([&spriteRenderer, &program, &sprites]() {
        program.clearScreen();
        for (auto& sprite : sprites) {
            (*sprite)
                .move(rand() % 3 - 1, rand() % 3 - 1)
                .rotate(rand() % 3 - 1);

            spriteRenderer.addSprite(sprite);
        }
        spriteRenderer.draw();
    });
	window.run();

    return 0;
}