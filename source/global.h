
#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifdef _WIN32
#define NOMINMAX
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif

#include <string>
namespace Global {
    std::string GetExeDir();
    void SetExeDir(std::string dir);
}

#endif