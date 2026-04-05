#pragma once
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <unordered_map>

#include <nbkit/singleton.h>
#include <terme/input_manager/base_input_manager.h>

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

    Display* display = nullptr;

public:
    LinuxInputManager()
    {
        XInitThreads();
        display = XOpenDisplay(nullptr);
    }

    ~LinuxInputManager()
    {
        if (display != nullptr)
        {
            XCloseDisplay(display);
            display = nullptr;
        }
    }

    inline bool IsKeyPressed(Key key) override
    {
        if (display == nullptr)
            return false;

        auto mapping = kKeyMap.find(key);
        if (mapping == kKeyMap.end())
            return false;

        const KeyCode keyCode = XKeysymToKeycode(display, mapping->second);
        if (keyCode == 0)
            return false;

        char keys[32] = {0};
        XLockDisplay(display);
        XQueryKeymap(display, keys);
        XUnlockDisplay(display);

        const int byteIndex = keyCode / 8;
        const int bitIndex = keyCode % 8;
        return (keys[byteIndex] & (1 << bitIndex)) != 0;
    }

    inline bool IsAnyKeyPressed() override
    {
        if (display == nullptr)
            return false;

        char keys[32] = {0};
        XLockDisplay(display);
        XQueryKeymap(display, keys);
        XUnlockDisplay(display);

        for (const char keyState : keys)
        {
            if (keyState != 0)
                return true;
        }

        return false;
    }
};

} // namespace terme
