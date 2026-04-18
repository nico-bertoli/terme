#pragma once

#include "terme/core/frame.h"
#include "terme/printers/printer.h"

namespace terme
{
    class UIPrinter : public Printer
    {
        //---------------------------------------------------------- fields

    public:
        enum class WindowPosition { kCenterXTopY, kCenterXCenterY };

    private:
        static constexpr char kMarginHorizontalChar = '=';
        static constexpr char kMarginVerticalChar = '|';

        //---------------------------------------------------------- methods
    public:
        UIPrinter(size_t screen_size_x, size_t screen_size_y, size_t screen_padding, TerminalColor margins_color);
        void PrintWindow(const Frame& window, TerminalColor color, WindowPosition window_position);
        void PrintOnHeader(const std::string& header, int x_pos, TerminalColor color);

    private:
        void DrawMargins(TerminalColor color);
        void DrawHorizontalMargin();
    };
}