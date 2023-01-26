#ifndef WINDOW_H_
#define WINDOW_H_

#include "globalIncludes.h"
#include "error.h"
#include "windowedApplication.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

class Event;

using string = std::string;
using ivec2  = glm::ivec2;

class Window {
public:
#ifdef _WIN32
    typedef HINSTANCE HInstance;
#else
    typedef void* HInstance;
#endif
    Window(HInstance hinstance, WindowedApplication& application);
    void close();

    bool isError();
    ErrorType getErrorType();
    string getErrorMessage();
    
    void endLoop();
    void run();

    const ivec2 getDimensions();
private:
    string               _name;
    GLFWwindow*          _window = NULL;
    string               _errorMessage;
    ErrorType            _errorType = ErrorType::NONE;
    ivec2                _size;
    bool                 _done = false;
    HInstance            _hinstance;
    WindowedApplication& _application;

    static void _handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
    static void _handleResize(GLFWwindow* window, int width, int height);
    static void _handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void _handleText(GLFWwindow* window, unsigned int codepoint);
    static void _handleCursor(GLFWwindow* window, double xpos, double ypos);
    static void _handleCursorEnter(GLFWwindow* window, int entered);
    static void _handleMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void _handleScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void _handleWindowRefresh(GLFWwindow *window);
    
    void _populateIcon();
    void _render();

    Window& _updateSize(int width, int height);
    Window& _setError(const ErrorType& errorType, const string &message);
    Window& _initGlfw();
    Window& _initGl();
    Window& _init();
    Window& _start();
    Window& _handleEvent(const Event& e);
};

#endif

