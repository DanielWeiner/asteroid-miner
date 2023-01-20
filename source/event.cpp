#include "event.h"

using namespace std;

EventType Event::type()
{
    switch(_event->type) {
    case SDL_QUIT:
        return EventType::QUIT;
    case SDL_WINDOWEVENT:
        switch(_event->window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            return EventType::RESIZE;
        default:
            return EventType::UNKNOWN;
        }
    default:
        return EventType::UNKNOWN;
    }
}

Event::Event(SDL_Event *event) : _event(event)
{
}