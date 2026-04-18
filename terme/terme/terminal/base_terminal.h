#pragma once

#include "terme/general/vector2_int.h"

#include <iostream>
#include <string>

namespace terme
{
    using TerminalColor = const char*;
    namespace color
    {
        static constexpr const char* kBlack   = "\033[30m";
        static constexpr const char* kRedDark = "\033[31m";
        static constexpr const char* kGreenDark = "\033[32m";
        static constexpr const char* kYellowDark = "\033[33m";
        static constexpr const char* kBlueDark = "\033[34m";
        static constexpr const char* kMagentaDark = "\033[35m";
        static constexpr const char* kCyanDark = "\033[36m";
        static constexpr const char* kWhiteDark = "\033[37m";

        static constexpr const char* kGrey   = "\033[90m";
        static constexpr const char* kRed    = "\033[91m";
        static constexpr const char* kGreen  = "\033[92m";
        static constexpr const char* kYellow = "\033[93m";
        static constexpr const char* kBlue   = "\033[94m";
        static constexpr const char* kMagenta = "\033[38;2;255;0;255m";
        static constexpr const char* kCyan   = "\033[96m";
        static constexpr const char* kWhite  = "\033[97m";

        static constexpr const char* kUserTerminal  = "\033[0m";

        // background colors (standard)
        static constexpr const char* kBgBlack   = "\033[40m";
        static constexpr const char* kBgRedDark = "\033[41m";
        static constexpr const char* kBgGreenDark = "\033[42m";
        static constexpr const char* kBgYellowDark = "\033[43m";
        static constexpr const char* kBgBlueDark = "\033[44m";
        static constexpr const char* kBgMagentaDark = "\033[45m";
        static constexpr const char* kBgCyanDark = "\033[46m";
        static constexpr const char* kBgWhiteDark = "\033[47m";

        // background colors (bright)
        static constexpr const char* kBgGrey   = "\033[100m";
        static constexpr const char* kBgRed    = "\033[101m";
        static constexpr const char* kBgGreen  = "\033[102m";
        static constexpr const char* kBgYellow = "\033[103m";
        static constexpr const char* kBgBlue   = "\033[104m";
        static constexpr const char* kBgMagenta = "\033[105m";
        static constexpr const char* kBgCyan   = "\033[106m";
        static constexpr const char* kBgWhite  = "\033[107m";
        
        static constexpr const char* kBgUserTerminal = "\033[49m";
    }

    ////////////////////////////////////////////////////////////////// base_terminal
    class BaseTerminal
    {
    protected:
        const char* front_color_;
        const char* back_color_;

        const char* default_front_color_ = color::kWhite;
        const char* default_back_color_ = color::kBgBlack;

        //================================================================= ansi color constants

    public:
        virtual ~BaseTerminal(){SetColor(color::kUserTerminal, color::kBgUserTerminal);}

        const char* GetFrontColor(){ return front_color_; };
        const char* GetBackColor(){ return back_color_; };
        void SetColor(const char* front_color = nullptr, const char* back_color = nullptr);
        void SetDefaultColors(const char* front_color, const char* back_color);

        virtual void Clear() = 0;
        virtual void SetCursorPosition(const vector2_int& position) = 0;
        virtual void SetCursorPosition(int x, int y){SetCursorPosition(vector2_int(x,y));}

        void Cout(std::string_view s) const;
        void Cout(char c) const;
        void Flush() const { std::cout << std::flush; } //grants cout termination before proceeding

    protected:
        BaseTerminal() : front_color_(color::kWhite), back_color_(color::kBgBlack), default_back_color_(color::kBgBlack) { HideCursor(); }
        virtual void HideCursor() {}
    };
}
