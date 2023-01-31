#ifndef WIN_CONSOLE_H_
#define WIN_CONSOLE_H_
#ifdef _WIN32

#include "global.h"

#include <iostream>
#include <cstdio>

namespace WinConsole {
    bool RedirectConsoleIO();
    bool ReleaseConsole();
    void AdjustConsoleBuffer(int16_t minLength = 1024);

    bool CreateNewConsole(int16_t minLength = 1024);

    bool AttachParentConsole(int16_t minLength = 1024);
}
#endif
#endif