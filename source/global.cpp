#include "global.h"

#include <chrono>

namespace {
    std::string exeDir = "";
}

std::string Global::GetExeDir()
{
    return exeDir.c_str();
}

void Global::SetExeDir(std::string dir)
{
    exeDir = dir;
}
