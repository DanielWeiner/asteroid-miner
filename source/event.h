#ifndef EVENT_H_
#define EVENT_H_

class Window;
#include <GLFW/glfw3.h>
#include <initializer_list>
#include <optional>

struct MouseButton {
    static const int
        BUTTON_LEFT   = GLFW_MOUSE_BUTTON_LEFT,
        BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
        BUTTON_RIGHT  = GLFW_MOUSE_BUTTON_RIGHT,

        BUTTON_LAST   = GLFW_MOUSE_BUTTON_LAST,
    
        BUTTON_0      = GLFW_MOUSE_BUTTON_1,
        BUTTON_1      = GLFW_MOUSE_BUTTON_2,
        BUTTON_2      = GLFW_MOUSE_BUTTON_3,
        BUTTON_3      = GLFW_MOUSE_BUTTON_4,
        BUTTON_4      = GLFW_MOUSE_BUTTON_5,
        BUTTON_5      = GLFW_MOUSE_BUTTON_6,
        BUTTON_6      = GLFW_MOUSE_BUTTON_7,
        BUTTON_7      = GLFW_MOUSE_BUTTON_8;
};

struct KeyModifier {
    static const int
        KEY_MOD_SHIFT     = GLFW_MOD_SHIFT,
        KEY_MOD_CONTROL   = GLFW_MOD_CONTROL,
        KEY_MOD_ALT       = GLFW_MOD_ALT,
        KEY_MOD_SUPER     = GLFW_MOD_SUPER,
        KEY_MOD_CAPS_LOCK = GLFW_MOD_CAPS_LOCK,
        KEY_MOD_NUM_LOCK  = GLFW_MOD_NUM_LOCK;
};

struct Key {
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
};

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
    struct PartialEvent {
        const std::optional<EventType>    type;
        const std::optional<EventAction>  action;
        const std::optional<int>          key;
        const std::optional<int>          scancode;
        const std::optional<unsigned int> codepoint;
        const std::optional<double>       x;
        const std::optional<double>       y;
        const std::optional<int>          button;
        const std::optional<double>       xoffset;
        const std::optional<double>       yoffset;
        const std::optional<int>          width;
        const std::optional<int>          height;
        const std::optional<int>          mods;
    };
public:
    const EventType    type{EventType::UNKNOWN};
    const EventAction  action{EventAction::UNKNOWN};
    const int          key{-1};
    const int          scancode{-1};
    const unsigned int codepoint{(unsigned int)-1};
    const double       x{-1};
    const double       y{-1};
    const int          button{-1};
    const double       xoffset{-1};
    const double       yoffset{-1};
    const int          width{-1};
    const int          height{-1};
    const int          mods{-1};

    bool isButtonPressed(const int targetButton) const;

    const Event add(const PartialEvent&& other) const;
};

#endif