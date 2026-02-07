#pragma once
#include <terme/terminal/base_terminal.h>
#include <nbkit/singleton.h>
#include <termios.h>
#include <optional>

namespace terme
{
    class LinuxTerminal : public nbkit::Singleton<LinuxTerminal>, public BaseTerminal
    {
        friend class nbkit::Singleton<LinuxTerminal>;

        static constexpr const char* kClearTerminal = "\033[2J\033[3J";

    private:
        std::optional<termios> original_termios_;

    protected:
        LinuxTerminal();
        ~LinuxTerminal();

    public:
        void Clear() override;
        void SetCursorPosition(const vector2_int& position) override;

    protected:
        void HideCursor() override;
        void ShowCursor();

    public:
        using BaseTerminal::SetCursorPosition;
    };
}