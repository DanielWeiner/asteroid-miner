#ifndef WINDOW_H_
#define WINDOW_H_

#include "globalIncludes.h"
#include "point.h"
#include "error.h"

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>

#include <string>
#include <functional>

#include <memory>

class Event;

class Window {
public:
    Window(const char* name, const Dimension &dimension);

    
    void close();

    bool isError();
    ErrorType getErrorType();
    std::string getErrorMessage();


    void setRenderLoop(std::function<void()> renderLoop);
    void setEventLoop(std::function<void(Event&)> eventLoop);
    
    void endLoop();

    void run();
private:
    const char                 *_name;
    SDL_Window                 *_window = NULL;
    std::string                 _errorMessage;
    ErrorType                   _errorType = ErrorType::NONE;
    Dimension                   _size;
    SDL_GLContext               _glContext = NULL;
    std::function<void()>       _renderLoop;
    std::function<void(Event&)> _eventLoop;
    bool                        _done = false;

    Window& _setError(const ErrorType& errorType, const std::string &message);
    Window& _initSdl();
    Window& _initGl();
    Window& _init();
    Window& _start();
};

#endif

