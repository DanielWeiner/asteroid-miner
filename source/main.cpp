#include "globalIncludes.h"

#include "event.h"
#include "point.h"
#include "window.h"

int main(int argc, char *argv[])  {
#ifdef _WIN32
    SetProcessDPIAware();
    timeBeginPeriod(1);
#endif

	Window window = Window("Asteroid Miner", Dimension(640., 480.));
	window.setEventLoop([&window](Event &event) {
		if (event.type() == EventType::QUIT) {
			window.endLoop();
		}
        if (event.type() == EventType::RESIZE) {
            auto dimensions = window.getDimensions();
            window.setName("(" + to_string(static_cast<int>(dimensions.width())) + ", " + to_string(static_cast<int>(dimensions.height())) + ")");
        }
	});
	window.run();

    return 0;
}