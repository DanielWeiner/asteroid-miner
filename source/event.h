#ifndef EVENT_H_
#define EVENT_H_

class Window;

using namespace std;

enum class EventType {
    UNKNOWN,

    WINDOW,
    KEY,
    TEXT,
    MOUSE
};

enum class EventAction {
    UNKNOWN,

    RESIZE,
    PRESS,
    RELEASE,
    REPEAT,
    ENTER,
    LEAVE,
    HOVER,
    SCROLL
};

class Event {
    friend Window;
public:
    const EventType    type;
    const EventAction  action;
    const int          key;
    const int          scancode;
    const unsigned int codepoint;
    const double       x;
    const double       y;
    const int          button;
    const double       xoffset;
    const double       yoffset;
    const int          width;
    const int          height;
    const int          mods;
};

#endif