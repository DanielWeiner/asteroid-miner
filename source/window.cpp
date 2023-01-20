#include "window.h"

#include "event.h"
#include "point.h"

#include "utility"

Window::Window(const char* name, const Dimension& dimension):
    _name(name),
    _size(dimension),
    _renderLoop([]() {}),
    _eventLoop([](Event&) {})
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
        //Destroy window	
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    //Quit SDL subsystems
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

void Window::setRenderLoop(const std::function<void()> renderLoop)
{
    _renderLoop = renderLoop;
}

void Window::setEventLoop(const std::function<void(Event&)> eventLoop)
{
    _eventLoop = eventLoop;
}

void Window::endLoop()
{
    _done = true;
}

Window& Window::_start()
{
    SDL_Event e;
    SDL_StartTextInput();

    while (!_done) {
        while (SDL_PollEvent(&e) != 0) {
            Event event(new SDL_Event(e));
            _eventLoop(event);
        }

        _renderLoop();

        SDL_GL_SwapWindow(_window);
    }

    //Disable text input
    SDL_StopTextInput();

    return *this;
}

void Window::run() {
    _init();
    _start();
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
        _name,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _size.width(),
        _size.height(),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
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

    return *this;
}


Window& Window::_initGl() {
    return *this;
}
