#ifndef ERROR_H_
#define ERROR_H_

#include <unordered_map>

using namespace std;

enum ErrorType {
    NONE,
    ERR_GLFW_INIT,
    ERR_GLFW_CREATE_WINDOW,
    ERR_GLAD_LOADER,
    ERR_SDL_CREATE_CONTEXT,
    ERR_GLEW_INIT
};

namespace AM_Error {
    const unordered_map<ErrorType, const char*> errorNames{
        { ErrorType::NONE,                   "" },
        { ErrorType::ERR_GLFW_INIT,          "GLFW initialization error" },
        { ErrorType::ERR_GLFW_CREATE_WINDOW, "GLFW window creation error" },
        { ErrorType::ERR_GLAD_LOADER,        "GLAD loader error" },
        { ErrorType::ERR_GLEW_INIT,          "GLEW initialization error" }
    };
}



#endif