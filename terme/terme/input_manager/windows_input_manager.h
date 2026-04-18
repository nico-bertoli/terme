#pragma once

#include "terme/input_manager/base_input_manager.h"

#include <unordered_map>

#include <nbkit/singleton.h>

#include <windows.h>

namespace terme
{

class WindowsInputManager : public nbkit::Singleton<WindowsInputManager>, public BaseInputManager
{
private:
    inline static const std::unordered_map<Key, int> kKeyMap =
    {
        {Key::kSpace, VK_SPACE},
        {Key::kW, 0x57},
        {Key::kA, 0x41},
        {Key::kS, 0x53},
        {Key::kD, 0x44},
        {Key::kArrowUp, VK_UP},
        {Key::kArrowDown, VK_DOWN},
        {Key::kArrowLeft, VK_LEFT},
        {Key::kArrowRight, VK_RIGHT},
        {Key::kNum0, 0x30},
        {Key::kNum1, 0x31},
        {Key::kNum2, 0x32},
        {Key::kNum3, 0x33},
        {Key::kNum4, 0x34},
        {Key::kNum5, 0x35},
        {Key::kNum6, 0x36},
        {Key::kNum7, 0x37},
        {Key::kNum8, 0x38},
        {Key::kNum9, 0x39},
        {Key::kEsc, VK_ESCAPE}
    };
    
public:
    inline bool IsKeyPressed(Key key) override
    {
        if (kKeyMap.find(key) != kKeyMap.end())
            return GetAsyncKeyState(kKeyMap.at(key)) & 0x8000;

        return false;
    }

    inline bool IsAnyKeyPressed() override
    {
        for (int i = 0; i < 256; ++i)
            if (GetAsyncKeyState(i) & 0x8000)
                return true;

        return false;
    }
};

} // namespace terme