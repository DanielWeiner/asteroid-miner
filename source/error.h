#ifndef ERROR_H_
#define ERROR_H_

#include <unordered_map>

using namespace std;

enum ErrorType {
    NONE,

    ERR_SDL_INIT_VIDEO,
    ERR_SDL_CREATE_WINDOW,
    ERR_SDL_CREATE_CONTEXT,
    ERR_GLEW_INIT
};

namespace AM_Error {
    const unordered_map<ErrorType, const char*> errorNames{
        { ErrorType::NONE,                   "" },
        { ErrorType::ERR_SDL_INIT_VIDEO,     "SDL initialization error" },
        { ErrorType::ERR_SDL_CREATE_WINDOW,  "SDL window initialization error" },
        { ErrorType::ERR_SDL_CREATE_CONTEXT, "SDL context initialization error" },
        { ErrorType::ERR_GLEW_INIT,          "GLEW initialization error" }
    };
}



#endif