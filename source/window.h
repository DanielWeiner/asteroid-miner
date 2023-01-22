#ifndef WINDOW_H_
#define WINDOW_H_

#include "globalIncludes.h"
#include "error.h"

#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <SDL_opengl.h>
#include <gl/glu.h>
#include <glm/vec2.hpp>

#include <string>
#include <functional>
#include <memory>

class Event;

using string = std::string;
using ivec2  = glm::ivec2;

class Window {
public:
    Window(string name, ivec2 dimension);

    void close();

    bool isError();
    ErrorType getErrorType();
    string getErrorMessage();

    void setName(string name);
    void setRenderLoop(std::function<void()> renderLoop);
    void setEventLoop(std::function<void(Event&)> eventLoop);
    void onInit(std::function<void()> initCallback);
    
    void endLoop();
    void run();

    const ivec2 getDimensions();
private:
    string                 _name;
    SDL_Window*            _window = NULL;
    string                 _errorMessage;
    ErrorType              _errorType = ErrorType::NONE;
    ivec2                  _size;
    SDL_GLContext          _glContext = NULL;
    function<void()>       _renderLoop;
    function<void(Event&)> _eventLoop;
    function<void()>       _onInit;
    bool                   _done = false;

    static unordered_map<Uint32, Window*> _instances;
    static int _handeWindowEvent(void* data, SDL_Event* event);
    static void _handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

    Window& _updateSize();
    Window& _setError(const ErrorType& errorType, const string &message);
    Window& _initSdl();
    Window& _initGl();
    Window& _init();
    Window& _start();
    Window& _handleEvent(SDL_Event* e);
};

#endif

