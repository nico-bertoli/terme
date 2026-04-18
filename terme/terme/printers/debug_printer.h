#pragma once

#include "terme/printers/printer.h"
#include "terme/terminal/terminal.h"

#include <iostream>
#include <string>

namespace terme
{
    class DebugPrinter : public Printer
    {
        //------------------------------------------ settings
    private:
        static constexpr TerminalColor kDebugColor = color::kCyan;
        static constexpr size_t kFpsPosition = 1;
        static constexpr size_t kPositionCoutCalls = 2;
        static constexpr size_t kGenericLogPosition = 3;

        //------------------------------------------ methods
    public:
        using Printer::Printer;

    public:
        void PrintCoutCallsCount(size_t cout_calls_count);
        void PrintFpsString(size_t fps);
        void PrintGenericLog(const std::string& log, int line_index);

    };
}