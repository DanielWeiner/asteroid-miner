#include "window.h"

#include "event.h"

#include <utility>
#include <iostream>

int Window::_handeWindowEvent(void* data, SDL_Event* event) {
    Window* sourceWindow = static_cast<Window*>(data);
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
        if (win == sourceWindow->_window) {
            sourceWindow->_updateSize();
        }
    }
    sourceWindow->_handleEvent(event);
    return 0;
}

void Window::_handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << message << std::endl;
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
    if (_initSdl().isError())
        return *this;
    if (_initGl().isError())
        return *this;
    return *this;
}

void Window::close()
{
    if (_window != NULL) {	
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    SDL_Quit();
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
    SDL_SetWindowTitle(_window, _name.c_str());
}

void Window::setRenderLoop(const std::function<void()> renderLoop) {
    _renderLoop = renderLoop;
}

void Window::setEventLoop(const std::function<void(Event&)> eventLoop)
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
    SDL_StartTextInput();
    SDL_AddEventWatch(_handeWindowEvent, this);
    glDebugMessageCallback(_handleError, this);

    _onInit();
    while (!_done) {
        SDL_PumpEvents();
        _renderLoop();
        SDL_GL_SwapWindow(_window);
    }

    SDL_DelEventWatch(_handeWindowEvent, this);
    //Disable text input
    SDL_StopTextInput();

    return *this;
}

Window& Window::_handleEvent(SDL_Event* e)
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

Window& Window::_updateSize()
{
    int width, height;
    SDL_GL_GetDrawableSize(_window, &width, &height);
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

Window& Window::_initSdl() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return _setError(ErrorType::ERR_SDL_INIT_VIDEO, SDL_GetError());
    }

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    _window = SDL_CreateWindow(
        _name.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _size.x,
        _size.y,
        0 
        | SDL_WINDOW_OPENGL 
        | SDL_WINDOW_RESIZABLE 
        | SDL_WINDOW_SHOWN 
        | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if (_window == NULL) {
        return _setError(ErrorType::ERR_SDL_CREATE_WINDOW, SDL_GetError());
    }

    _glContext = SDL_GL_CreateContext(_window);
    if (_glContext == NULL) {
        return _setError(ErrorType::ERR_SDL_CREATE_CONTEXT, SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        return _setError(ErrorType::ERR_GLEW_INIT, reinterpret_cast<const char*>(glewGetErrorString(glewError)));
    }

    //Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    _updateSize();

    return *this;
}


Window& Window::_initGl() {
    return *this;
}
