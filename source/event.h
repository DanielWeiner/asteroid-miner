#ifndef EVENT_H_
#define EVENT_H_

#include <memory>
#include <SDL2/SDL.h>

class Window;

using namespace std;

enum EventType {
    UNKNOWN,

    QUIT
};

class Event {
    friend Window;
public:
    EventType type();

private:
    const shared_ptr<SDL_Event> _event;
    
    Event(SDL_Event *event);
};

#endif