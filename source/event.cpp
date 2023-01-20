#include "event.h"

using namespace std;

EventType Event::type()
{
    switch(_event->type) {
    case SDL_QUIT:
        return EventType::QUIT;
    default:
        return EventType::UNKNOWN;
    }
}

Event::Event(SDL_Event *event) : _event(shared_ptr<SDL_Event>(event))
{
}