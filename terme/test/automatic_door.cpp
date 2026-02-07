#include "automatic_door.h"

using Direction = terme::Direction;

namespace Platformer
{
    AutomaticDoor::AutomaticDoor
    (
        int x_pos,
        int y_pos,
        size_t size_x,
        size_t size_y,
        char model_char,
        double move_speed
    ) : VariableSizeCollider(x_pos, y_pos, size_x, size_y, model_char), move_speed_(move_speed)
    {
        starting_pos_y_ = y_pos;
    }

    void AutomaticDoor::Update()
    {
        VariableSizeCollider::Update();
        if (IsOpen())
        {
            if (GetPosY() < starting_pos_y_ + GetModelHeight())
                TryMove(Direction::kUp, move_speed_);
        }
        else
        {
            if (GetPosY() > starting_pos_y_)
                TryMove(Direction::kDown, move_speed_);
        }
    }

    void AutomaticDoor::RemoveEnergySource()
    {
        if (energy_sources_counter_ > 0)
            --energy_sources_counter_;
    }
}