#include "window.h"
#include "event.h"

#include <utility>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <string>
#include <filesystem>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "shellapi.h"
#endif

namespace {
    int         _currentErrorCode;
    std::string _currentErrorMessage;
    
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
}

void Window::_handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << message << std::endl;
}

void Window::_handleResize(GLFWwindow* window, int width, int height)
{
    _windowInstances[window]->_updateSize(width, height);
    _windowInstances[window]->_handleEvent({
        .type = EventType::WINDOW,
        .action = EventAction::RESIZE,
        .width = width,
        .height = height
    });
}

void Window::_handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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
        .type = EventType::KEY,
        .action = eventAction,
        .key = key,
        .scancode = scancode,
        .mods = mods
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
        .type = EventType::MOUSE,
        .action = EventAction::HOVER,
        .x = xpos,
        .y = ypos
    });
}

void Window::_handleCursorEnter(GLFWwindow* window, int entered)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    _windowInstances[window]->_handleEvent({
        .type = EventType::MOUSE,
        .action = entered ? EventAction::ENTER : EventAction::LEAVE,
        .x = x,
        .y = y
    });
}

void Window::_handleMouseButton(GLFWwindow* window, int button, int action, int mods)
{
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
                if (_buttonStates[button].x == x && _buttonStates[button].y == y) {
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
        .type = EventType::MOUSE,
        .action = eventAction,
        .x = x,
        .y = y,
        .button = button,
        .mods = mods
    });

    switch(additionalAction) {
        case EventAction::CLICK:
            _windowInstances[window]->_handleEvent({
                .type = EventType::MOUSE,
                .action = additionalAction,
                .x = x,
                .y = y,
                .button = button,
                .mods = mods
            });
            return;
        case EventAction::DRAG:
            _windowInstances[window]->_handleEvent({
                .type = EventType::MOUSE,
                .action = additionalAction,
                .x = x,
                .y = y,
                .button = button,
                .xoffset = _buttonStates[button].x - x,
                .yoffset = _buttonStates[button].y - y,
                .mods = mods
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
        .type = EventType::MOUSE,
        .action = EventAction::SCROLL,
        .x = x,
        .y = y,
        .xoffset = xoffset,
        .yoffset = yoffset
    });
}

void Window::_handleWindowRefresh(GLFWwindow* window)
{
    _windowInstances[window]->_render();
}

void Window::_render() {
    if (_application.getName() != _name) {
        _name = _application.getName();

        glfwSetWindowTitle(_window, _application.getName());
    }
    _application.render();
    glfwSwapBuffers(_window);
}

void Window::_populateIcon() 
{
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(_window);
    TCHAR filename[MAX_PATH + 1];
    GetModuleFileName(NULL, filename, MAX_PATH);
    HICON icon = ExtractIcon((HINSTANCE)_hinstance, filename, 0);
    SendMessage(hwnd, (UINT)WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(hwnd, (UINT)WM_SETICON, ICON_BIG, (LPARAM)icon);
#endif
}

Window::Window(HInstance hinstance, WindowedApplication& application):
    _name(application.getName()),
    _size(ivec2(application.getWidth(), application.getHeight())),
    _hinstance(hinstance),
    _application(application)
{
}

Window& Window::_init() {
    if (_initGlfw().isError())
        return *this;
    if (_initGl().isError())
        return *this;
    return *this;
}

void Window::close()
{
    if (_window != NULL) {	
        glfwDestroyWindow(_window);
        _window = NULL;
    }
    
}

bool Window::isError()
{
    return _errorType != ErrorType::NONE;
}

ErrorType Window::getErrorType()
{
    return _errorType;
}

std::string Window::getErrorMessage()
{
    return _errorMessage;
}

void Window::endLoop()
{
    _done = true;
}

Window& Window::_start()
{
    glDebugMessageCallback(_handleError, this);

    _application.init();
    while (!glfwWindowShouldClose(_window)) {
        
        glfwPollEvents();
        _render();
    }
    
    _windowInstances.erase(_window);

    glfwTerminate();

    return *this;
}

Window& Window::_handleEvent(const Event& e)
{
    _application.handleEvent(e);

    return *this;
}

void Window::run() {
    if (_init().isError())
        return;
    _start();
}

Window& Window::_updateSize(int width, int height)
{
    _size = ivec2(width, height);

    glViewport(0, 0, width, height);

    return *this;
}

Window& Window::_setError(const ErrorType& errorType, const std::string& message)
{
    _errorType = errorType;
    _errorMessage = message;

    return *this;
}

Window& Window::_initGlfw() {
    static bool handlersInitialized = false;
    if (!handlersInitialized) {
        glfwSetErrorCallback(_handleGlfwError);
        handlersInitialized = true;
    }
    if (glfwInit() == GLFW_FALSE) {
        return _setError(ErrorType::ERR_GLFW_INIT, _currentErrorMessage);
    }

    //Use OpenGL 3.3 core   
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef TARGET_OS_X
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    _window = glfwCreateWindow(
        _size.x,
        _size.y,
        _name.c_str(),
        NULL,
        NULL
    );
    if (_window == NULL) {
        return _setError(ErrorType::ERR_GLFW_CREATE_WINDOW, _currentErrorMessage);
    }

    _populateIcon();

    glfwMakeContextCurrent(_window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return _setError(ErrorType::ERR_GLAD_LOADER, "GLAD loader error");
    }

    std::cout << "OpenGL version: %s\n", glGetString(GL_VERSION);

    _windowInstances[_window] = this;

    glfwSetFramebufferSizeCallback(_window, _handleResize);
    glfwSetKeyCallback(_window, _handleKey);
    glfwSetCharCallback(_window, _handleText);
    glfwSetCursorPosCallback(_window, _handleCursor);
    glfwSetCursorEnterCallback(_window, _handleCursorEnter);
    glfwSetMouseButtonCallback(_window, _handleMouseButton);
    glfwSetScrollCallback(_window, _handleScroll);
    glfwSetWindowRefreshCallback(_window, _handleWindowRefresh);

    //Use Vsync
    glfwSwapInterval(1);

    glfwMaximizeWindow(_window);

    return *this;
}


Window& Window::_initGl() {
    return *this;
}
