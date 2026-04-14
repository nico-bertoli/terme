#include <terme/printers/ui_printer.h>

using std::string;

namespace terme
{
    UIPrinter::UIPrinter(size_t screen_size_x, size_t screen_size_y, size_t screen_padding, TerminalColor margins_color) :
        Printer(screen_size_x, screen_size_y, screen_padding)
    {
        DrawMargins(margins_color);
    };

    void UIPrinter::PrintWindow(const Frame& window, TerminalColor color, WindowPosition window_position)
    {
        assert(window.GetSizeY() > 0);
        assert(screen_size_x_ > window.GetSizeX());
        size_t left_window_offset = (screen_size_x_ - window.GetSizeX()) / 2;

        size_t print_start_y;
        switch (window_position)
        {
        case WindowPosition::kCenterXCenterY:
            print_start_y = screen_size_y_ / 2 - window.GetSizeY() / 2;
            break;

        case WindowPosition::kCenterXTopY:
            print_start_y = kTopMarginSize;
            break;
        }

        Terminal::Instance().SetColor(color);
        for (size_t y = 0; y < window.GetSizeY(); ++y)
        {
            string line = "";
            for (size_t x = 0; x < window.GetSizeX(); ++x)
                line += window.chars.Get(x, y);

            terminal_.SetCursorPosition(left_window_offset + kLeftMarginSize, y + print_start_y);
            terminal_.Cout(line);
        }
        terminal_.Flush();
    }

    void UIPrinter::PrintOnHeader(const string& header, int x_pos, TerminalColor color)
    {
        terminal_.SetColor(color);
        terminal_.SetCursorPosition(x_pos, 0);
        terminal_.Cout(header);
    }

    void UIPrinter::DrawMargins(TerminalColor color)
    {
        terminal_.SetColor(color);

        //horizontal
        terminal_.SetCursorPosition(0, 1);
        DrawHorizontalMargin();
        terminal_.SetCursorPosition(0, static_cast<int>(GetMaxTerminalY()));
        DrawHorizontalMargin();

        //vertical
        for (size_t y = kTopMarginSize; y < GetMaxTerminalY(); ++y)
        {
            terminal_.SetCursorPosition(size_t(0), y);
            terminal_.Cout(kMarginVerticalChar);
            terminal_.SetCursorPosition(GetMaxTerminalX(), y);
            terminal_.Cout(kMarginVerticalChar);
        }
    }

    void UIPrinter::DrawHorizontalMargin()
    {
        string line = "";
        for (int x = 0; x < screen_size_x_ + 2; ++x)
            line += kMarginHorizontalChar;
        terminal_.Cout(line);
    }
}