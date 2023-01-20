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

using namespace std;

class Window {
public:
    Window(string name, const Dimension &dimension);

    void close();

    bool isError();
    ErrorType getErrorType();
    std::string getErrorMessage();

    void setName(string name);
    void setRenderLoop(function<void()> renderLoop);
    void setEventLoop(function<void(Event&)> eventLoop);
    
    void endLoop();
    void run();

    const Dimension getDimensions();
private:
    string                 _name;
    SDL_Window*            _window = NULL;
    string                 _errorMessage;
    ErrorType              _errorType = ErrorType::NONE;
    Dimension              _size;
    SDL_GLContext          _glContext = NULL;
    function<void()>       _renderLoop;
    function<void(Event&)> _eventLoop;
    bool                   _done = false;

    static unordered_map<Uint32, Window*> _instances;
    static int _handeWindowEvent(void* data, SDL_Event* event);

    Window& _updateSize();
    Window& _setError(const ErrorType& errorType, const string &message);
    Window& _initSdl();
    Window& _initGl();
    Window& _init();
    Window& _start();
    Window& _handleEvent(SDL_Event* e);
};

#endif

