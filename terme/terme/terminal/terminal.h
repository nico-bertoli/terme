#pragma once
#include <terme/settings.h>

 #if IS_LINUX_BUILD
    #include <terme/terminal/linux_terminal.h>
 #else
    #include <terme/terminal/windows_terminal.h>
 #endif

namespace terme
{
    #if IS_LINUX_BUILD
        using Terminal = LinuxTerminal;
    #else
        using Terminal = WindowsTerminal;
    #endif
}