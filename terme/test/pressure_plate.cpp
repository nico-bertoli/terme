#include "pressure_plate.h"
#include <terme/managers/audio_manager.h>

using std::shared_ptr;
using Direction = terme::Direction;

namespace Platformer
{
    PressurePlate::PressurePlate(int x_pos, int y_pos, size_t width) : Collider(x_pos, y_pos)
    {
        unpressed_model_ = CreteModelUsingChar('=', width, 1);
        pressed_model_ = CreteModelUsingChar('_', width, 1);
    }

    void PressurePlate::OnCollisionEnter(shared_ptr<Collider> other, Direction collision_dir)
    {
        if (collision_dir == Direction::kUp && collisions_[Direction::kUp].size() == 1)
        {
            on_press.Notify();
            SetModel(pressed_model_);
            terme::AudioManager::Instance().PlayFx("resources/button_press.wav");
        }
    }

    void PressurePlate::OnCollisionExit(Direction ending_collision_dir)
    {
        if (ending_collision_dir == Direction::kUp && collisions_[Direction::kUp].size() == 0)
        {
            on_release.Notify();
            SetModel(unpressed_model_);
            terme::AudioManager::Instance().PlayFx("resources/sounds/button_release.wav");
        }

    }
}