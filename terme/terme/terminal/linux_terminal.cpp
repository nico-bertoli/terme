#include "terme/settings.h"

#if IS_LINUX_BUILD

#include "terme/terminal/terminal.h"

#include <cstring>
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>

using std::cout;

namespace terme
{
    LinuxTerminal::LinuxTerminal()
    {
        termios original;
        if (tcgetattr(STDIN_FILENO, &original) == 0)
        {
            // save original termios
            original_termios_ = original;
            
            termios new_termios;
            std::memcpy(&new_termios, &original, sizeof(termios));
            
            // prevents characters from being printed
            new_termios.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        }
        
        HideCursor();
    }

    LinuxTerminal::~LinuxTerminal()
    {
        ShowCursor();
        if (original_termios_.has_value())
            tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_.value());
    }

    void LinuxTerminal::HideCursor()
    {
        std::cout << "\033[?25l";
        std::cout.flush();
    }

    void LinuxTerminal::ShowCursor()
    {
        std::cout << "\033[?25h";
        std::cout.flush();
    }
    
    void LinuxTerminal::Clear()
    {
        cout << kClearTerminal;
        cout << "\033[2J\033[H"; //apply bg color
        SetCursorPosition(0, 0);
        HideCursor();
    }

    void LinuxTerminal::SetCursorPosition(const vector2_int& position)
    {
        std::cout << "\033[" << (position.y + 1) << ';' << (position.x + 1) << 'H';
    }
}

#endif