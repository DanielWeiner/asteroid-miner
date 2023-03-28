#ifndef WINDOW_H_
#define WINDOW_H_

#include "global.h"
#include "error.h"
#include "windowedApplication.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class Event;

using string = std::string;

class Window {
public:
    enum class Cursor {
        HAND,
        ARROW,
        IBEAM,
        CROSSHAIR,
        HRESIZE,
        VRESIZE
    };

    Window(string name, float width, float height);
    void close();

    bool isError();
    ErrorType getErrorType();
    string getErrorMessage();
    
    void setTitle(std::string title);
    void addApplication(WindowedApplication& app);
    void setCursor(Cursor cursor) const;
    
    void endLoop();
    void run();

    glm::vec2 getSize() const;
    int getDpi() const;
private:
    using AppArray = std::vector<std::reference_wrapper<WindowedApplication>>;

    string      _title;
    GLFWwindow* _window = NULL;
    string      _errorMessage;
    ErrorType   _errorType = ErrorType::NONE;
    glm::vec2   _size;
    bool        _done = false;
    AppArray    _applications;

    static int   _getClickedMouseButton(GLFWwindow* window);
    static void  _handleError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
    static void  _handleResize(GLFWwindow* window, int width, int height);
    static void  _handleKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void  _handleText(GLFWwindow* window, unsigned int codepoint);
    static void  _handleCursor(GLFWwindow* window, double xpos, double ypos);
    static void  _handleCursorEnter(GLFWwindow* window, int entered);
    static void  _handleMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void  _handleScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void  _handleWindowRefresh(GLFWwindow *window);
    
    void _populateIcon();
    void _render();

    Window& _updateSize(int width, int height);
    Window& _setError(const ErrorType& errorType, const string &message);
    Window& _start();
    Window& _handleEvent(const Event& e);

    GLFWcursor* _cursors[6];
};

#endif

