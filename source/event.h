#ifndef EVENT_H_
#define EVENT_H_

class Window;

namespace Key {
    static const int 
        KEY_SPACE = 32,
        KEY_A     = 65,
        KEY_B     = 66,
        KEY_C     = 67,
        KEY_D     = 68,
        KEY_E     = 69,
        KEY_F     = 70,
        KEY_G     = 71,
        KEY_H     = 72,
        KEY_I     = 73,
        KEY_J     = 74,
        KEY_K     = 75,
        KEY_L     = 76,
        KEY_M     = 77,
        KEY_N     = 78,
        KEY_O     = 79,
        KEY_P     = 80,
        KEY_Q     = 81,
        KEY_R     = 82,
        KEY_S     = 83,
        KEY_T     = 84,
        KEY_U     = 85,
        KEY_V     = 86,
        KEY_W     = 87,
        KEY_X     = 88,
        KEY_Y     = 89,
        KEY_Z     = 90,
        KEY_ESC   = 256,
        KEY_ENTER = 257;
}

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

    HOVER,
    SCROLL,

    PRESS,
    RELEASE,
    REPEAT,

    CLICK,
    DRAG,

    ENTER,
    LEAVE
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