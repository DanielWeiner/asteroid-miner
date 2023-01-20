#ifndef EVENT_H_
#define EVENT_H_

#include <SDL2/SDL.h>

class Window;

using namespace std;

enum EventType {
    UNKNOWN,

    QUIT,
    RESIZE
};

class Event {
    friend Window;
public:
    EventType type();

private:
    SDL_Event* _event;
    
    Event(SDL_Event *event);
};

#endif