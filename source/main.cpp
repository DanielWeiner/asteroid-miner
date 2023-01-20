#include "globalIncludes.h"

#include "event.h"
#include "point.h"
#include "window.h"

int main(int argc, char *argv[])  {
#ifdef _WIN32
    timeBeginPeriod(1);
#endif

	Window window = Window("Asteroid Miner", Dimension(640., 480.));
	window.setEventLoop([&window](Event &event) {
		if (event.type() == EventType::QUIT) {
			window.endLoop();
		}
	});
	window.run();

    return 0;
}