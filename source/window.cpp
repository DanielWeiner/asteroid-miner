#include "window.h"

#include "event.h"

#include <utility>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <string>

namespace {
    int         _currentErrorCode;
    std::string _currentErrorMessage;
    
    std::unordered_map<GLFWwindow*, Window*> _windowInstances;
    
    void _handleGlfwError(int errorCode, const char* message) {
        _currentErrorCode = errorCode;
        _currentErrorMessage = message;
    }
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
    EventAction eventAction;
    switch (action) {
        case GLFW_PRESS:
            eventAction = EventAction::PRESS;
            break;
        case GLFW_RELEASE:
            eventAction = EventAction::RELEASE;
            break;
        default:
            eventAction = EventAction::UNKNOWN;
    }
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    _windowInstances[window]->_handleEvent({
        .type = EventType::MOUSE,
        .action = eventAction,
        .x = x,
        .y = y,
        .button = button,
        .mods = mods
    });
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

Window::Window(std::string name, ivec2 dimension):
    _name(name),
    _size(dimension),
    _renderLoop([]() {}),
    _eventLoop([](Event&) {}),
    _onInit([](){})
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

void Window::setName(string name) 
{
    _name = name;
    if (_window == NULL)
        return;
    glfwSetWindowTitle(_window, _name.c_str());
}

void Window::setRenderLoop(const std::function<void()> renderLoop) {
    _renderLoop = renderLoop;
}

void Window::setEventLoop(const std::function<void(const Event&)> eventLoop)
{
    _eventLoop = eventLoop;
}

void Window::onInit(std::function<void()> initCallback) 
{
    _onInit = initCallback;
}

void Window::endLoop()
{
    _done = true;
}

Window& Window::_start()
{
    glDebugMessageCallback(_handleError, this);

    _onInit();
    while (!glfwWindowShouldClose(_window)) {
        _renderLoop();
        glfwPollEvents();
        glfwSwapBuffers(_window);
    }
    
    _windowInstances.erase(_window);

    glfwTerminate();

    return *this;
}

Window& Window::_handleEvent(const Event& e)
{
     Event event(e);
    _eventLoop(event);

    return *this;
}

void Window::run() {
    if (_init().isError())
        return;
    _start();
}

const ivec2 Window::getDimensions()
{
    return ivec2(_size);
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

    //Use Vsync
    glfwSwapInterval(1);

    _updateSize(_size.x, _size.y);

    return *this;
}


Window& Window::_initGl() {
    return *this;
}
