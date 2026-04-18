#include "terme/printers/simulation_printer.h"
#include "terme/core/simulation.h"
#include "terme/entities/collider.h"
#include "terme/entities/game_object.h"
#include "terme/entities/level.h"
#include "terme/managers/debug_manager.h"
#include "terme/managers/time_manager.h"

#include <algorithm>
#include <cassert>

using std::string;

namespace terme
{
    SimulationPrinter::SimulationPrinter
    (
        size_t screen_size_x,
        size_t screen_size_y,
        size_t screen_padding,
        TerminalColor bg_chars_color,
        const char* background_file_name
    ) : Printer(screen_size_x, screen_size_y, screen_padding), bg_chars_color_(bg_chars_color)
    {
        InitBackground(background_file_name);
    }

    void SimulationPrinter::PrintObject(GameObject* go)
    {
        Model model = go->GetModel();
        assert(model.GetSizeX() > 0 && model.GetSizeY() > 0);
        PrintInternal(go->GetPosX(), go->GetPosY(), go->GetModelWidth(), go->GetModelHeight(), go);
    }

    void SimulationPrinter::ClearObject(GameObject* obj)
    {
        PrintInternal(obj->GetPosX(), obj->GetPosY(), obj->GetModelWidth(), obj->GetModelHeight(), nullptr);
    }

    void SimulationPrinter::ClearArea(int world_x_pos, int world_y_pos, size_t x_size, size_t y_size)
    {
        PrintInternal(world_x_pos, world_y_pos, x_size, y_size, nullptr);
    }

    void SimulationPrinter::PrintInternal(int world_x_pos, int world_y_pos, size_t x_size, size_t y_size, GameObject* go)
    {
        terminal_.SetColor(go == nullptr ? nullptr : go->GetColor(), go == nullptr ? nullptr : go->GetBackColor());
        for (int y_screen = ConvertWorldPosToScreenPos(world_y_pos), y_model = 0; y_model < y_size && y_screen < screen_size_y_; ++y_screen, ++y_model)
        {
            if (y_screen + kTopMarginSize < kTopMarginSize) continue;
            int line_terminal_pos_x = ConvertWorldPosToScreenPos(world_x_pos) + kLeftMarginSize;
            string line = "";

            for (int x_screen = ConvertWorldPosToScreenPos(world_x_pos), x_model = 0; x_model < x_size && x_screen < screen_size_x_; ++x_screen, ++x_model)
            {
                if (x_screen + kLeftMarginSize < kLeftMarginSize)continue;

                if (go == nullptr)
                    line += background_.IsSetup() ? background_.chars_.Get(x_screen, screen_size_y_ - y_screen - 1) : ' ';
                else
                    line += go->GetModel().Get(x_model, go->GetModelHeight() - 1 - y_model);
            }
            if (line_terminal_pos_x < kLeftMarginSize)
                line_terminal_pos_x = kLeftMarginSize;
            terminal_.SetCursorPosition(line_terminal_pos_x, screen_size_y_ - y_screen + kBottomMarginSize);
            terminal_.Cout(line);
        }
    }

    void SimulationPrinter::PrintBackground()
    {
        if (!background_.IsSetup())
            return;

        terminal_.SetColor(bg_chars_color_);

        string line = "";


        for (int y = 0; y < screen_size_y_; ++y)
        {
            for (int x = 0; x < screen_size_x_; ++x)
            {
                //reversing y order
                char char_to_print = background_.chars_.Get(x, y);
                line += char_to_print;
            }
            terminal_.SetCursorPosition(kLeftMarginSize, y + kTopMarginSize);
            terminal_.Cout(line);
            line.clear();
        }
    }

    void SimulationPrinter::InitBackground(const char* background_file_name)
    {
        if (background_file_name[0] == '\0')
            return;

        background_.ReadFromFile(background_file_name);
        PrintBackground();
    }
}