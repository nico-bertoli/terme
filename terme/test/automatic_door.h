#pragma once
// #include "config.h"
#include <terme/entities/variable_size_collider.h>

namespace Platformer
{
    class AutomaticDoor : public terme::VariableSizeCollider
    {
        //---------------------------------------------------------- Fields
    private:
        double move_speed_;
        int starting_pos_y_;
        int energy_sources_counter_ = 0;

        //---------------------------------------------------------- Methods
    public:
        AutomaticDoor
        (
            int x_pos,
            int y_pos,
            size_t size_x,
            size_t size_y,
            char model_char,
            double move_speed
        );

        bool IsOpen() const { return energy_sources_counter_ > 0; }
        bool CanExitScreenSpace() const override { return false; }
        double GetGravityScale() const override { return 0; }
        terme::TerminalColor GetColor() const { return terme::color::kCyan; }
        void AddEnergySource() { ++energy_sources_counter_; }
        void RemoveEnergySource();

    protected:
        void Update() override;
    };
}