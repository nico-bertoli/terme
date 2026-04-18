#pragma once

#include "terme/settings.h"

#if IS_LINUX_BUILD
#include "terme/input_manager/linux_input_manager.h"
#else
#include "terme/input_manager/windows_input_manager.h"
#endif

namespace terme
{
    #if IS_LINUX_BUILD
        using InputManager = LinuxInputManager;
    #else
        using InputManager = WindowsInputManager;
    #endif
}
