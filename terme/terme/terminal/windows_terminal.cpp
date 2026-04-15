#include <terme/settings.h>

#if !IS_LINUX_BUILD

#include <terme/terminal/windows_terminal.h>
#include <terme/terminal/base_terminal.h>
#include <terme/managers/debug_manager.h>
#include <terme/general/vector2_int.h>

#include <iostream>
#include <string>
#include <windows.h>

using string = std::string;

namespace terme
{
    void WindowsTerminal::Clear()
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = { 0, 0 };
        DWORD cCharsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dwConSize;

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

        FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coord, &cCharsWritten);

        GetConsoleScreenBufferInfo(hConsole, &csbi);
        FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coord, &cCharsWritten);

        SetCursorPosition(coord.X,coord.Y);
    }

    void WindowsTerminal::SetCursorPosition(const vector2_int& position)
    {
        COORD coord;
        coord.X = static_cast<short>(position.X);
        coord.Y = static_cast<short>(position.Y);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void WindowsTerminal::HideCursor()
    {
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
        cursorInfo.bVisible = false; // hide cursor
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    }
}

#endif