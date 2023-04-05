#include "window.h"
#include "winConsole.h"
#include "event.h"

#include <utility>
#include <iostream>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <cmath>
#include <format>
#include <string>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "shellapi.h"
#endif

namespace {
    const int CLICK_THRESHOLD = 5;
    int         _currentErrorCode;
    std::string _currentErrorMessage;
    int lastMods = 0;
    
    std::unordered_map<GLFWwindow*, Window*> _windowInstances;
    
    void _handleGlfwError(int errorCode, const char* message) {
        _currentErrorCode = errorCode;
        _currentErrorMessage = message;
    }

    struct ButtonState {
        bool pressed = false;
        float x = 0.f;
        float y = 0.f;
    };

    ButtonState _buttonStates[GLFW_MOUSE_BUTTON_LAST + 1];

    bool initializeHandlers(void (&messageHandler)(int, const char*)) {
        static bool initialized = false;
        if (!initialized) {
            glfwSetErrorCallback(messageHandler);
            initialized = true;
        }

        return true;
    }
}

void Window::setCursor(Window::Cursor cursor) const {
    glfwSetCursor(_window, _cursors[(int)cursor]);
}

int Window::_getClickedMouseButton(GLFWwindow *window) {
    int buttonsPressed = 0;

    for (int button{GLFW_MOUSE_BUTTON_1}; button <= GLFW_MOUSE_BUTTON_LAST; button++) {
        if (glfwGetMouseButton(window, button) == GLFW_PRESS) {
            buttonsPressed |= 1 << button;
        }
    }

    return buttonsPressed;
 }

void Window::_handleError(GLenum source, GLenum type, GLuint id,
                          GLenum severity, GLsizei length,
                          const GLchar *message, const void *userParam) {
        std::cout << message << std::endl;
}

void Window::_handleResize(GLFWwindow* window, int width, int height)
{
    _windowInstances[window]->_handleEvent({
        .type   = EventType::WINDOW,
        .action = EventAction::RESIZE,
        .width  = width,
        .height = height
    });
}

void Window::_handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    lastMods = mods;        
    EventAction eventAction;
    switch (action) {
        case GLFW_PRESS:
            eventAction = EventAction::PRESS;
            break;
        case GLFW_REPEAT:
            eventAction = EventAction::REPEAT;
            break;
        case GLFW_RELEASE:
            eventAction = EventAction::RELEASE;
            break;
        default:
            eventAction = EventAction::UNKNOWN;
    }

    _windowInstances[window]->_handleEvent({
        .type     = EventType::KEY,
        .action   = eventAction,
        .key      = key,
        .scancode = scancode
    });
}

void Window::_handleText(GLFWwindow* window, unsigned int codepoint)
{
    _windowInstances[window]->_handleEvent({
        .type = EventType::TEXT,
        .codepoint = codepoint
    });
}

void Window::_handleCursor(GLFWwindow* window, double xpos, double ypos)
{
    _windowInstances[window]->_handleEvent({
        .type   = EventType::MOUSE,
        .action = EventAction::HOVER,
    });
}

void Window::_handleCursorEnter(GLFWwindow* window, int entered)
{
    _windowInstances[window]->_handleEvent({
        .type   = EventType::MOUSE,
        .action = entered ? EventAction::ENTER : EventAction::LEAVE,
    });
}

void Window::_handleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    lastMods = mods;
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    EventAction eventAction;
    EventAction additionalAction = EventAction::UNKNOWN;
    switch (action) {
        case GLFW_PRESS:
            eventAction = EventAction::PRESS;

            _buttonStates[button].pressed = true;
            _buttonStates[button].x = x;
            _buttonStates[button].y = y;
            break;
        case GLFW_RELEASE:
            eventAction = EventAction::RELEASE;

            if (_buttonStates[button].pressed) {
                if (abs(_buttonStates[button].x - x) <= CLICK_THRESHOLD && abs(_buttonStates[button].y - y) <= CLICK_THRESHOLD) {
                    additionalAction = EventAction::CLICK;
                } else {
                    additionalAction = EventAction::DRAG;
                }
            }

            _buttonStates[button].pressed = false;
            break;
        default:
            eventAction = EventAction::UNKNOWN;
    }

    _windowInstances[window]->_handleEvent({
        .type   = EventType::MOUSE,
        .action = eventAction
    });

    switch(additionalAction) {
        case EventAction::CLICK:
            _windowInstances[window]->_handleEvent({
                .type   = EventType::MOUSE,
                .action = additionalAction
            });
            return;
        case EventAction::DRAG:
            _windowInstances[window]->_handleEvent({
                .type    = EventType::MOUSE,
                .action  = additionalAction,
                .xoffset = _buttonStates[button].x - x,
                .yoffset = _buttonStates[button].y - y
            });
            return;
        default:
            return;
    } 
}

void Window::_handleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    _windowInstances[window]->_handleEvent({
        .type    = EventType::MOUSE,
        .action  = EventAction::SCROLL,
        .xoffset = xoffset,
        .yoffset = yoffset
    });
}

void Window::_handleWindowRefresh(GLFWwindow* window)
{
    _windowInstances[window]->_render();
}

void Window::_render() const 
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (WindowedApplication& application : _applications) {
        application.render();
    }
    glfwSwapBuffers(_window);
}

Window::Window(const string name, const float width, const float height, const AppArray& applications) :
    _window([name, width, height, this](){
        GLFWwindow* window = nullptr;
        if (glfwInit() == GLFW_FALSE) {
            return window;
        }

        initializeHandlers(_handleGlfwError);

        glfwWindowHint(GLFW_SAMPLES, 4);

        //Use OpenGL 4.3 core   
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef TARGET_OS_X
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        window = glfwCreateWindow(
            width,
            height,
            name.c_str(),
            NULL,
            NULL
        );
        if (window == NULL) {
            return window;
        }

#ifdef _WIN32
        HWND hwnd = glfwGetWin32Window(window);
        TCHAR filename[MAX_PATH + 1];
        GetModuleFileName(NULL, filename, MAX_PATH);
        HICON icon = ExtractIcon(GetModuleHandle(NULL), filename, 0);
        SendMessage(hwnd, (UINT)WM_SETICON, ICON_SMALL, (LPARAM)icon);
        SendMessage(hwnd, (UINT)WM_SETICON, ICON_BIG, (LPARAM)icon);
#endif
        glfwMakeContextCurrent(window);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwTerminate();

            return window;
        }

        glfwSetFramebufferSizeCallback(window, _handleResize);
        glfwSetKeyCallback(window, _handleKey);
        glfwSetCharCallback(window, _handleText);
        glfwSetCursorPosCallback(window, _handleCursor);
        glfwSetCursorEnterCallback(window, _handleCursorEnter);
        glfwSetMouseButtonCallback(window, _handleMouseButton);
        glfwSetScrollCallback(window, _handleScroll);
        glfwSetWindowRefreshCallback(window, _handleWindowRefresh);

        //Use Vsync
        glfwSwapInterval(1);
        
        glfwMaximizeWindow(window);

        _windowInstances[window] = this;

        return window;
    }()),
    _applications(applications),
    _initialWidth(width),
    _initialHeight(height),
    _cursors{
        glfwCreateStandardCursor(GLFW_HAND_CURSOR),
        glfwCreateStandardCursor(GLFW_ARROW_CURSOR),
        glfwCreateStandardCursor(GLFW_IBEAM_CURSOR),
        glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR),
        glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR),
        glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR)
    }
{}

void Window::close()
{
    if (_window != NULL) {
        glfwDestroyWindow(_window);
        _window = NULL;
    }
}

void Window::setTitle(std::string title) const
{
    glfwSetWindowTitle(_window, title.c_str());
}

void Window::endLoop() const { 
    glfwSetWindowShouldClose(_window, true);
}

void Window::_start() const
{
    glDebugMessageCallback(_handleError, this);

    for (WindowedApplication& application : _applications) {
        application.init();
    }
    
    glViewport(0, 0, _initialWidth, _initialHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glEnable(GL_MULTISAMPLE);
    
    while (!glfwWindowShouldClose(_window)) {
        glfwPollEvents();
        _render();
    }
    
    _windowInstances.erase(_window);

    glfwTerminate();
}

void Window::_handleEvent(const Event& e) const
{
    
    // Add mouse x, y and buttons to event
    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    auto mouseButton = _getClickedMouseButton(_window);

    for (WindowedApplication& application : _applications) {
        application.handleEvent(e.add({
            .x = x, 
            .y = y,
            .button = mouseButton,
            .mods = lastMods
        }));
    }
}

void Window::run() const {
    _start();
}

glm::vec2 Window::getSize() const
{
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    return glm::vec2(width, height);
}

int Window::getDpi() const {
    float scaleX, scaleY;
    
    glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &scaleX, &scaleY);

    return (int)(scaleX * 96); //TODO: make sure this is platform agnostic
}