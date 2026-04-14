#include <string>

#include <terme/managers/debug_manager.h>
#include <terme/printers/debug_printer.h>
#include <terme/general/vector2_int.h>

using std::string;

namespace terme
{
    void DebugPrinter::PrintCoutCallsCount(size_t cout_calls_count)
    {
        TerminalColor originalFrontColor = Terminal::Instance().GetFrontColor();
        TerminalColor originalBackColor = Terminal::Instance().GetBackColor();

        Terminal::Instance().SetColor(kDebugColor);
        terminal_.SetCursorPosition(0, GetMaxTerminalY() + kPositoinCoutCalls);
        //final space makes sure old string is cleared
        string output = "COUT CALLS: " + std::to_string(cout_calls_count) + string("     ");
        std::cout << output;

        terminal_.SetColor(originalFrontColor, originalBackColor);
    }

    void DebugPrinter::PrintFpsString(size_t fps)
    {
        terminal_.SetColor(kDebugColor);
        terminal_.SetCursorPosition(0, GetMaxTerminalY() + kFpsPosition);
        string output = "FPS: " + std::to_string(fps) + '\n';
        std::cout << output;
    }

    void DebugPrinter::PrintGenericLog(const string& log, int line_index)
    {
        terminal_.SetColor(kDebugColor);
        terminal_.SetCursorPosition(0, GetMaxTerminalY() + kGenericLogPosition + line_index);
        std::cout << log;
        std::cout.flush();
    }
}