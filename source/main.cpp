#include "globalIncludes.h"

#include "event.h"
#include "spriteRenderer.h"
#include "window.h"

int main(int argc, char *argv[])  {
#ifdef _WIN32
    SetProcessDPIAware();
    timeBeginPeriod(1);
#endif

	Window window = Window("Asteroid Miner", { 640, 480 });
    ShaderProgram program;
    SpriteRenderer spriteRenderer(&program, &window);
	window.setEventLoop([&window](Event &event) {
		if (event.type() == EventType::QUIT) {
			window.endLoop();
		}
        if (event.type() == EventType::RESIZE) {
            auto dimensions = window.getDimensions();
            window.setName("(" + to_string(dimensions.x) + ", " + to_string(dimensions.y) + ")");
        }
	});
    

    window.onInit([&spriteRenderer]() {
        spriteRenderer.init();
    });

    window.setRenderLoop([&spriteRenderer, &program]() {
        program.clearScreen();
        spriteRenderer.draw();
    });
	window.run();

    return 0;
}