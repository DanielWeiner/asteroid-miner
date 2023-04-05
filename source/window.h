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
using AppArray = std::vector<std::reference_wrapper<WindowedApplication>>;

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

    Window(
        const string    name, 
        const float     width, 
        const float     height,
        const AppArray& _applications
    );
    void close();

    void setTitle(std::string title) const;
    void setCursor(Cursor cursor) const;
    
    void endLoop() const;
    void run() const;

    glm::vec2 getSize() const;
    int       getDpi()  const;
private:
    

    GLFWwindow*     _window = NULL;
    bool            _done = false;
    const AppArray& _applications;
    const int       _initialWidth;
    const int       _initialHeight;
    const string    _initialTitle;

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
    
    void _render() const;

    void _start() const;
    void _handleEvent(const Event& e) const;

    GLFWcursor* _cursors[6];
};

#endif

