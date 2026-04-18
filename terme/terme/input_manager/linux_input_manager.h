#pragma once

#include "terme/input_manager/base_input_manager.h"

#include <unordered_map>

#include <nbkit/singleton.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

namespace terme
{

class LinuxInputManager : public nbkit::Singleton<LinuxInputManager>, public BaseInputManager
{
private:
    inline static const std::unordered_map<Key, KeySym> kKeyMap =
    {
        {Key::kSpace,      XK_space},
        {Key::kW,          XK_w},
        {Key::kA,          XK_a},
        {Key::kS,          XK_s},
        {Key::kD,          XK_d},
        {Key::kArrowUp,   XK_Up},
        {Key::kArrowDown, XK_Down},
        {Key::kArrowLeft, XK_Left},
        {Key::kArrowRight,XK_Right},
        {Key::kNum0,      XK_0},
        {Key::kNum1,      XK_1},
        {Key::kNum2,      XK_2},
        {Key::kNum3,      XK_3},
        {Key::kNum4,      XK_4},
        {Key::kNum5,      XK_5},
        {Key::kNum6,      XK_6},
        {Key::kNum7,      XK_7},
        {Key::kNum8,      XK_8},
        {Key::kNum9,      XK_9},
        {Key::kEsc,        XK_Escape}
    };

    Display* display_ = nullptr;

public:
    LinuxInputManager()
    {
        XInitThreads();
        display_ = XOpenDisplay(nullptr);
    }

    ~LinuxInputManager()
    {
        if (display_ != nullptr)
        {
            XCloseDisplay(display_);
            display_ = nullptr;
        }
    }

    inline bool IsKeyPressed(Key key) override
    {
        if (display_ == nullptr)
            return false;

        auto mapping = kKeyMap.find(key);
        if (mapping == kKeyMap.end())
            return false;

        const KeyCode key_code = XKeysymToKeycode(display_, mapping->second);
        if (key_code == 0)
            return false;

        char keys[32] = {0};
        XLockDisplay(display_);
        XQueryKeymap(display_, keys);
        XUnlockDisplay(display_);

        const int byte_index = key_code / 8;
        const int bit_index = key_code % 8;
        return (keys[byte_index] & (1 << bit_index)) != 0;
    }

    inline bool IsAnyKeyPressed() override
    {
        if (display_ == nullptr)
            return false;

        char keys[32] = {0};
        XLockDisplay(display_);
        XQueryKeymap(display_, keys);
        XUnlockDisplay(display_);

        for (const char key_state : keys)
        {
            if (key_state != 0)
                return true;
        }

        return false;
    }
};

} // namespace terme
