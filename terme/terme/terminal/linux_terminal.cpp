#include <terme/config.h>

#if IS_LINUX_BUILD

#include <terme/terminal/terminal.h>
#include <terme/managers/debug_manager.h>

#include <iostream>
#include <string>
#include <unistd.h> 
#include <termios.h>
#include <cstring>

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
            
            termios newTermios;
            std::memcpy(&newTermios, &original, sizeof(termios));
            
            // prevents characters from being printed
            newTermios.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);
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
        printf("\033[%d;%dH", position.Y + 1, position.X + 1);
    }
}

#endif