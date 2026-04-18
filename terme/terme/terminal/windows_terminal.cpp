#include "terme/settings.h"

#if !IS_LINUX_BUILD

#include "terme/general/vector2_int.h"
#include "terme/terminal/base_terminal.h"
#include "terme/terminal/windows_terminal.h"

#include <iostream>
#include <string>

#include <windows.h>

using string = std::string;

namespace terme
{
    void WindowsTerminal::Clear()
    {
        HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        DWORD c_chars_written;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dw_con_size;

        GetConsoleScreenBufferInfo(h_console, &csbi);
        dw_con_size = csbi.dwSize.X * csbi.dwSize.Y;

        FillConsoleOutputCharacter(h_console, ' ', dw_con_size, coord, &c_chars_written);

        GetConsoleScreenBufferInfo(h_console, &csbi);
        FillConsoleOutputAttribute(h_console, csbi.wAttributes, dw_con_size, coord, &c_chars_written);

        SetCursorPosition(coord.X, coord.Y);
    }

    void WindowsTerminal::SetCursorPosition(const vector2_int& position)
    {
        COORD coord;
        coord.X = static_cast<short>(position.x);
        coord.Y = static_cast<short>(position.y);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void WindowsTerminal::HideCursor()
    {
        CONSOLE_CURSOR_INFO cursor_info;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
        cursor_info.bVisible = false; // hide cursor
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
    }
}

#endif