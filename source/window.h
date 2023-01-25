#ifndef WINDOW_H_
#define WINDOW_H_

#include "globalIncludes.h"
#include "error.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <functional>

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
    void setEventLoop(std::function<void(const Event&)> eventLoop);
    void onInit(std::function<void()> initCallback);
    
    void endLoop();
    void run();

    const ivec2 getDimensions();
private:
    string                      _name;
    GLFWwindow*                 _window = NULL;
    string                      _errorMessage;
    ErrorType                   _errorType = ErrorType::NONE;
    ivec2                       _size;
    std::function<void()>       _renderLoop;
    std::function<void(Event&)> _eventLoop;
    std::function<void()>       _onInit;
    bool                        _done = false;

    static void _handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
    static void _handleResize(GLFWwindow* window, int width, int height);
    static void _handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _handleText(GLFWwindow* window, unsigned int codepoint);
    static void _handleCursor(GLFWwindow* window, double xpos, double ypos);
    static void _handleCursorEnter(GLFWwindow* window, int entered);
    static void _handleMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void _handleScroll(GLFWwindow* window, double xoffset, double yoffset);

    Window& _updateSize(int width, int height);
    Window& _setError(const ErrorType& errorType, const string &message);
    Window& _initGlfw();
    Window& _initGl();
    Window& _init();
    Window& _start();
    Window& _handleEvent(const Event& e);
};

#endif

