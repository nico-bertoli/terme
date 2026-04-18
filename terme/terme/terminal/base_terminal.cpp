#include "terme/terminal/base_terminal.h"
#include "terme/managers/debug_manager.h"

#include <cassert>

namespace terme
{
    void BaseTerminal::Cout(std::string_view s) const
    {
        std::cout << s;
        DebugManager::Instance().IncrementCoutCalls();
    }

    void BaseTerminal::Cout(char c) const
    {
        std::cout << c;
        DebugManager::Instance().IncrementCoutCalls();
    }

    void BaseTerminal::SetColor(const char* front_color, const char* back_color)
    {
        if(front_color == nullptr)
            front_color = default_front_color_;
        if(back_color == nullptr)
            back_color = default_back_color_;

        if(this->front_color_ != front_color)
        {
             std::cout<<front_color;
             this->front_color_ = front_color;
        }
        if(this->back_color_ != back_color)
        {
            std::cout<<back_color;
            this->back_color_ = back_color;
        }
    }

    void BaseTerminal::SetDefaultColors(const char* front_color, const char* back_color)
    {
        assert(front_color != nullptr);
        assert(back_color != nullptr);

        default_front_color_ = front_color;
        default_back_color_ = back_color;

        SetColor(default_front_color_, default_back_color_);
    }
}